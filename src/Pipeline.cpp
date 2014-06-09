//
//  Pipeline.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Pipeline.h"
#include "Node.h"
#include "Outlet.h"
#include "Inlet.h"
#include "PatchCord.h"
#include "FactoryNode.h"
#include <algorithm>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include "json/writer.h"


using namespace hm;
using namespace std;
typedef boost::unique_lock<boost::shared_mutex> UniqueLock;
typedef boost::shared_lock<boost::shared_mutex> SharedLock;
//typedef boost::upgrade_to_unique_lock<boost::shared_mutex> UpgradedLock;
//typedef boost::lock_guard<boost::mutex> Lock;

Pipeline::Pipeline()
: mIsRunning(false)
{
	hm_debug("Pipeline constructed");
}

Pipeline::~Pipeline()
{
	hm_debug("Pipeline destructor.");
	stop();
	mThread->join();
	clear();
}

std::string Pipeline::toString() const
{
	SharedLock lock(mPipelineMutex);
	stringstream ss;
	ss << *this;
	return ss.str();
}

std::vector<NodePtr> Pipeline::nodes() const
{
	SharedLock lock(mPipelineMutex);
	return mNodes;
}

std::vector<PatchCordPtr> Pipeline::patchCords() const
{
	SharedLock lock(mPipelineMutex);
	return vector<PatchCordPtr>(begin(mPatchCords), end(mPatchCords));
}

void Pipeline::addNode(NodePtr node)
{
	Events events;
	{
		UniqueLock lock(mPipelineMutex);
		events = p_AddNode(node);
	}
	p_Process(events);
}

void Pipeline::p_Process(Events const& events) const
{
	SharedLock listenerLock(mListenersMutex);
	for (EventPtr event: events)
	{
		for (Listener* listener: mListeners)
		{
			event->notify(listener);
		}
	}
}

Events Pipeline::p_AddNode(NodePtr node)
{
	Events events;
	// todo: assert mutex is locked
	mNodes.push_back(node);
//	node->setPipeline(this);
	hm_info("Added node "+node->toString()+" to pipeline.");
	if (isRunning() && !node->isProcessing())
	{
		node->startProcessing();
	}
	events.push_back(EventPtr(new NodeAddedEvent(node)));
	return events;
}

void Pipeline::removeNode(NodePtr node)
{
	Events events;
	{
		UniqueLock lock(mPipelineMutex);
		events = p_RemoveNode(node);
	}
	p_Process(events);
}

Events Pipeline::p_RemoveNode(NodePtr node)
{
	Events events;
	// todo: assert mutex is locked
	assert(node != nullptr);
	assert(p_PatchCordInvariant());
	// disconnect any patchcords. disconnect(PatchCordPtr) modifies
	// mPatchcords, so we make a copy of it before iterating over it.
	list<PatchCordPtr> patchcords = mPatchCords;
	for (PatchCordPtr p: patchcords)
	{
		for (InletPtr i: node->inlets())
		{
			if (p->inlet() == i)
			{
				Events e = p_Disconnect(p);
				events.insert(end(events), begin(e), end(e));
			}
		}
		for (OutletPtr o: node->outlets())
		{
			if (p->outlet() == o)
			{
				Events e = p_Disconnect(p);
				events.insert(end(events), begin(e), end(e));
			}
		}
	}
	cerr << "Stop and remove" << endl;
	// Stop and remove node
	{
		auto it = std::find(mNodes.begin(), mNodes.end(), node);
		if (it != mNodes.end())
		{
			if (isRunning())
			{
				assert(node->isProcessing());
				{
					node->stopProcessing();
				}
			}
//			node->setPipeline(nullptr);
			mNodes.erase(it);
		}
		events.push_back(EventPtr(new NodeRemovedEvent(node)));
		/// Check invariants
		for (OutletPtr outlet: node->outlets())
		{
			assert(outlet->numInlets()==0);
		}
		assert(p_PatchCordInvariant());
	}
	hm_info("Removed node "+node->toString()+" from pipeline.");
	return events;
}

void Pipeline::replaceNode(NodePtr oldNode, NodePtr newNode)
{
	assert(oldNode != nullptr);
	assert(newNode != nullptr);
	
	Events events;
	{
		UniqueLock lock(mPipelineMutex);
		
		// Remember what patch cords were attached
		vector<pair<OutletPtr, int>> cordsGoingIn;
		vector<pair<int, InletPtr>> cordsGoingOut;
		auto oldInlets = oldNode->inlets();
		auto oldOutlets = oldNode->outlets();
		for (PatchCordPtr p: mPatchCords)
		{
			for (int i=0; i<oldInlets.size(); i++)
			{
				if (p->inlet() == oldInlets[i])
				{
					cordsGoingIn.push_back(pair<OutletPtr, int>(p->outlet(), i));
				}
			}
			for (int i=0; i<oldOutlets.size(); i++)
			{
				if (p->outlet() == oldOutlets[i])
				{
					cordsGoingOut.push_back(pair<int, InletPtr>(i, p->inlet()));
				}
			}
		}
		
		{
			Events e = p_RemoveNode(oldNode);
			events.insert(end(events), begin(e), end(e));
		}
		{
			
			Events e = p_AddNode(newNode);
			events.insert(end(events), begin(e), end(e));
		}
		
		auto newInlets = newNode->inlets();
		for (auto const& p: cordsGoingIn)
		{
			if (p.second < newInlets.size())
			{
				Events e = p_Connect(p.first, newInlets[p.second]);
				events.insert(end(events), begin(e), end(e));
			}
		}
		auto newOutlets = newNode->outlets();
		for (auto const& p: cordsGoingOut)
		{
			if (p.first < newOutlets.size())
			{
				Events e = p_Connect(newOutlets[p.first], p.second);
				events.insert(end(events), begin(e), end(e));
			}
		}
	} // end of unique lock
	p_Process(events);
}

void Pipeline::start()
{
	mIsRunning = true;
	// TODO: Make this parallel
	// TODO: Create a wake condition and add a callback to inlets to discover
	// when nodes need waking up.
	for (auto p: mNodes)
	{
		p->startProcessing();
	}
	mIsRunning = true;
	mThread = std::unique_ptr<boost::thread>(new boost::thread([this]() {
		double timeOfLastUpdate = elapsedTime();
		while (mIsRunning)
		{
			// if characteristics have changed on a node, we need to act on
			// it with a unique lock
			std::list<NodePtr> nodesWithChangedCharacteristics;
			{
				SharedLock lock(mPipelineMutex);
				for (NodePtr node: mNodes)
				{
					if (node->stepProcessing())
					{
						nodesWithChangedCharacteristics.push_back(node);
					}
				}
			}
			// if node characteristics have changed then we might have
			// some dead patch cords (attached to a detached inlet/outlet).
			// we also need to notify any listeners with p_Process, but
			// this needs to happen when we don't have any locks active.
			if (!nodesWithChangedCharacteristics.empty())
			{
				Events events;
				{
					UniqueLock lock(mPipelineMutex);
					Events e = p_RemoveDeadPatchCords();
					assert(p_PatchCordInvariant());
					events.insert(end(events), begin(e), end(e));
				}
				for (NodePtr node: nodesWithChangedCharacteristics)
				{
					events.push_back(EventPtr(new NodeCharacteristicsChangedEvent(node)));
				}
				p_Process(events);
			}
			double delta = elapsedTime() - timeOfLastUpdate;
			timeOfLastUpdate += delta;
			// limit framerate to 100fps
			if (delta < 0.01)
			{
				int us = int((0.01 - delta) * 1000000);
				boost::this_thread::sleep_for(boost::chrono::microseconds(us));
			}
		}
		for (NodePtr node: mNodes)
		{
			node->stopProcessing();
		}
	}));
}

void Pipeline::stop()
{
	mIsRunning = false;
}


bool Pipeline::p_PatchCordInvariant() const
{
	// check inlets referred to in patch cords belong to nodes in this
	// pipelines
	for (auto p: mPatchCords)
	{
		if (!contains(mNodes, p->inlet()->node().lock()))
		{
			return false;
		}
		if (!contains(mNodes, p->outlet()->node().lock()))
		{
			return false;
		}
	}
	
	// check all patch cord outlets are aware of patch cord and correctly
	// report the connection
	for (auto p: mPatchCords)
	{
		if (!contains(p->outlet()->patchCords(), p))
		{
			return false;
		}
		if (!p->outlet()->isConnectedTo(p->inlet()))
		{
			return false;
		}
	}
	// check all patch cords registered at outlets are registered in mPatchCords
	for (NodePtr node: mNodes)
	{
		for (OutletPtr outlet: node->outlets())
		{
			for (PatchCordPtr p: outlet->patchCords())
			{
				if (!contains(mPatchCords, p))
				{
					return false;
				}
			}
		}
	}
	// check inlets have correct number of connections registered
	{
		std::map<InletPtr, int> inletNumConnectionsInNodes;
		std::map<InletPtr, int> inletNumConnectionsInPatchCords;
		for (NodePtr node: mNodes)
		{
			for (InletPtr inlet: node->inlets())
			{
				inletNumConnectionsInNodes[inlet] += inlet->numConnections();
				inletNumConnectionsInPatchCords[inlet] = 0;
			}
		}
		for (PatchCordPtr p: mPatchCords)
		{
			inletNumConnectionsInPatchCords[p->inlet()]++;
		}
		if (inletNumConnectionsInNodes != inletNumConnectionsInPatchCords)
		{
			return false;
		}
	}
	// check we have no duplicate patch cords
	for (auto i=mPatchCords.begin(); i!=mPatchCords.end(); ++i)
	{
		for (auto j=i; j!=mPatchCords.end(); ++j)
		{
			// i is iterator, *i is shared_ptr, **i is actual object
			if (i != j && **i == **j)
			{
				return false;
			}
		}
	}
	return true;
}

bool Pipeline::connect(OutletPtr outlet, InletPtr inlet)
{
	if (outlet==nullptr || inlet==nullptr || isConnected(outlet, inlet))
	{
		return false;
	}
	Events events;
	{
		UniqueLock lock(mPipelineMutex);
		events = p_Connect(outlet, inlet);
	}
	p_Process(events);
	
	// assert that if events is not empty then connection was a success
	assert([&]() {
		if (!events.empty())
		{
			shared_ptr<PatchCordAddedEvent> e = dynamic_pointer_cast<PatchCordAddedEvent>(events[0]);
			if (e)
			{
				return e->patchCord->inlet()==inlet && e->patchCord->outlet()==outlet;
			}
		}
		return false;
	}());

	return !events.empty();
}


Events Pipeline::p_Connect(OutletPtr outlet, InletPtr inlet)
{
	assert(outlet != nullptr && inlet != nullptr);
	Events events;
	assert(p_PatchCordInvariant());
	bool connectionFail(false);
	std::string failReason;
	if (p_IsConnected(outlet, inlet))
	{
		connectionFail = true;
		failReason = "they are already connected";
	}
	else if (!(outlet->types() & inlet->types()))
	{
		connectionFail = true;
		failReason = "they do not have any common data types";
	}
	if (connectionFail)
	{
		hm_info("Unable to connect "+outlet->toString()+" to "+inlet->toString()+" because "+failReason+'.');
		return events;
	}
	
	PatchCordPtr p(new PatchCord(outlet, inlet));
	outlet->addPatchCord(p);
	inlet->incrementNumConnections();
	mPatchCords.push_back(p);
	assert(p_PatchCordInvariant());
	hm_debug("Connected "+outlet->toString()+" and "+inlet->toString());
	events.push_back(EventPtr(new PatchCordAddedEvent(p)));
	return events;
}


bool Pipeline::disconnect(OutletPtr outlet, InletPtr inlet)
{
	Events events;
	{
		UniqueLock lock(mPipelineMutex);
		events = p_Disconnect(outlet, inlet);
	}
	p_Process(events);
	
	// assert that if events is not empty then disconnection was a success
	assert([&]() {
		if (!events.empty())
		{
			shared_ptr<PatchCordRemovedEvent> e = dynamic_pointer_cast<PatchCordRemovedEvent>(events[0]);
			if (e)
			{
				return e->patchCord->inlet()==inlet && e->patchCord->outlet()==outlet;
			}
		}
		return false;
	}());
	
	return !events.empty();
}


PatchCordPtr Pipeline::p_FindPatchCord(OutletPtr outlet, InletPtr inlet) const
{
	auto it=find_if(begin(mPatchCords), end(mPatchCords), [&](PatchCordPtr p) {
		return p->outlet() == outlet && p->inlet() == inlet;
	});
	if (it==end(mPatchCords))
	{
		return nullptr;
	}
	else
	{
		return *it;
	}
}


Events Pipeline::p_Disconnect(OutletPtr outlet, InletPtr inlet)
{
	Events events;
	PatchCordPtr p = p_FindPatchCord(outlet, inlet);
	if (p == nullptr)
	{
		hm_info("Unable to disconnect "+outlet->toString()+" from "+inlet->toString()+" as they are not connected.");
		return Events();
	}
	return p_Disconnect(p);
}


Events Pipeline::p_Disconnect(PatchCordPtr p, bool skipInvariant)
{
	if (!skipInvariant)
	{
		assert(p_PatchCordInvariant());
	}
	// check p is a known patch cord
	assert(find(begin(mPatchCords), end(mPatchCords), p) != end(mPatchCords));
	p->outlet()->removePatchCord(p);
	p->inlet()->decrementNumConnections();
	mPatchCords.erase(find(mPatchCords.begin(), mPatchCords.end(), p));
	if (!skipInvariant)
	{
		assert(p_PatchCordInvariant());
	}
	hm_debug("Disconnected "+p->outlet()->toString()+" and "+p->inlet()->toString());
	Events events;
	events.push_back(EventPtr(new PatchCordRemovedEvent(p)));
	return events;
}


Events Pipeline::p_RemoveDeadPatchCords()
{
	Events events;
	for (auto it=begin(mPatchCords); it!=end(mPatchCords); ++it)
	{
		PatchCordPtr p = *it;
		if (p->inlet()->isDetached() || p->outlet()->isDetached())
		{
			Events e = p_Disconnect(p, true);
			events.insert(end(events), begin(e), end(e));
			// restart loop as mPatchCords is changed by p_Disconnect
			it = begin(mPatchCords);
		}
	}
	return events;
}


bool Pipeline::isConnected(OutletPtr outlet, InletPtr inlet) const
{
	SharedLock lock(mPipelineMutex);
	return p_IsConnected(outlet, inlet);
}


bool Pipeline::p_IsConnected(OutletPtr outlet, InletPtr inlet) const
{
	// outlet is connected to inlet if there exists a patch cord containing
	// them both.
	return p_FindPatchCord(outlet, inlet) != nullptr;
}


void Pipeline::addListener(Listener* listener)
{
	UniqueLock lock(mListenersMutex);
	mListeners.push_back(listener);
	hm_debug("Added listener "<<listener);
}

bool Pipeline::removeListener(Listener* listener)
{
	UniqueLock lock(mListenersMutex);
	auto it = std::find(mListeners.begin(), mListeners.end(), listener);
	if (it==mListeners.end())
	{
		hm_warning("Failed to remove listener "<<listener<<" as it was not recognised.");
		return false;
	}
	else
	{
		mListeners.erase(it);
		hm_debug("Removed listener "<<listener);
		return true;
	}
}


NodePtr Pipeline::nodeFromPath(string path) const
{
	SharedLock lock(mPipelineMutex);
	return p_NodeFromPath(path);
}

NodePtr Pipeline::p_NodeFromPath(string path) const
{
	if (!path.empty() && path[0]=='/')
	{
		path = path.substr(1);
	}
	if (!path.empty() && path[path.size()-1]=='/')
	{
		path = path.substr(path.size()-1);
	}
	
	// Nodes are always at the root element of a path so the path equals
	// the nodename when stripped of any leading or trailing slashes
	if (path.empty())
	{
		hm_info("Unable to find node for empty path");
		return nullptr;
	}
	for (NodePtr n: mNodes)
	{
		if (n->name() == path)
		{
			return n;
		}
	}
	hm_info("No node found with name "+path);
	return nullptr;
}


vector<string> tokenizePath(string const& path)
{
	using namespace boost;
	tokenizer<char_separator<char>> tokens(path, char_separator<char>("/"));
	std::vector<std::string> splitPath;
	copy(tokens.begin(), tokens.end(), back_inserter(splitPath));
	return splitPath;
}


OutletPtr Pipeline::outletFromPath(std::string const& path) const
{
	SharedLock lock(mPipelineMutex);
	return p_OutletFromPath(path);
}

OutletPtr Pipeline::p_OutletFromPath(std::string const& path) const
{
	std::vector<std::string> splitPath = tokenizePath(path);
	if (splitPath.size() < 2)
	{
		hm_info("Path '"+path+"' does not contain both a node name and outlet name.");
		return nullptr;
	}
	NodePtr node = p_NodeFromPath(splitPath[0]);
	if (node)
	{
		for (OutletPtr outlet: node->outlets())
		{
			if (outlet->name() == splitPath[1])
			{
				return outlet;
			}
		}
		hm_info("Node "+node->name()+" of type "+node->type()+" does not " "have an outlet named "+splitPath[1]);
	}
	return nullptr;
}

InletPtr Pipeline::inletFromPath(std::string const& path) const
{
	SharedLock lock(mPipelineMutex);
	return p_InletFromPath(path);
}


InletPtr Pipeline::p_InletFromPath(std::string const& path) const
{
	std::vector<std::string> splitPath = tokenizePath(path);
	if (splitPath.size() < 2)
	{
		hm_info("Path '"+path+"' does not contain both a node name and inlet name.");
		return nullptr;
	}
	NodePtr node = p_NodeFromPath(splitPath[0]);
	if (node)
	{
		for (InletPtr inlet: node->inlets())
		{
			if (inlet->name() == splitPath[1])
			{
				return inlet;
			}
		}
		hm_info("Node "+node->name()+" of type "+node->type()+" does not " "have an inlet named "+splitPath[1]);
	}
	return nullptr;
}


void Pipeline::toJson(Json::Value& json) const
{
	SharedLock lock(mPipelineMutex);
	
	json = Json::Value();
	json["nodes"] = Json::Value(Json::arrayValue);
	for (int i=0; i<mNodes.size(); i++)
	{
		json["nodes"][i] << *mNodes[i];
	}
	json["patchcords"] = Json::Value(Json::arrayValue);
	int i=0;
	for (PatchCordPtr p: mPatchCords)
	{
		json["patchcords"][i] << *p;
		i++;
	}
}


void Pipeline::clear()
{
	Events events;
	{
		UniqueLock lock(mPipelineMutex);
		events = p_Clear();
	}
	p_Process(events);
}


Events Pipeline::p_Clear()
{
	Events events;
	hm_debug("Pipeline::clear(): Erasing all patchcords");
	// Do not iterate over containers as usual as disconnect/remove
	// will modify them.
	auto patchcords = mPatchCords;
	while (!mPatchCords.empty())
	{
		Events e = p_Disconnect(mPatchCords.back());
		events.insert(end(events), begin(e), end(e));
	}
	hm_debug("Pipeline::clear(): Erasing all nodes");
	while (!mNodes.empty())
	{
		Events e = p_RemoveNode(mNodes.back());
		events.insert(end(events), begin(e), end(e));
	}
	return events;
}


bool Pipeline::fromJsonString(std::string const& jsonString, vector<string>& errors)
{
	Json::Reader reader;
	Json::Value json;
	bool ok = reader.parse(jsonString, json);
	if (ok)
	{
		return fromJson(json, errors);
	}
	else
	{
		errors.push_back(reader.getFormatedErrorMessages());
		return false;
	}
}


bool Pipeline::fromJson(Json::Value const& json, vector<string>& errors)
{
	if (!json["nodes"].isArray())
	{
		errors.push_back("'nodes' array is missing.");
		return false;
	}
	if (!json["patchcords"].isArray())
	{
		errors.push_back("'patchcords' array is missing.");
		return false;
	}
	
	// Note - we do not lock the pipeline when loading json - just
	// use the public interface.
	
//	Events events;
	{
//		UniqueLock lock(mPipelineMutex);
		
//		bool wasRunning = isRunning();
//		if (wasRunning)
//		{
//			stop();
//		}
//		{
//			Events e = p_Clear();
		clear();
//			events.insert(end(events), begin(e), end(e));
//		}
		FactoryNode& factory = *FactoryNode::instance();
		for (auto& jNode: json["nodes"])
		{
			string type = jNode["type"].asString();
			if (type.empty())
			{
				errors.push_back("Node element found in JSON with no 'type' value. Skipping this node.");
				continue;
			}
			if (!factory.hasNodeType(type))
			{
				errors.push_back("Node element found in JSON with unrecognised node type: '"+type+"'. Skipping this node.");
				continue;
			}
			Node::Params nodeParams;
			if (!(jNode["settings"] >> nodeParams))
			{
				errors.push_back("Failed to parse 'settings' element for node of type '"+type+"'. Skipping this node.");
				continue;
			}
			NodePtr node = factory.create(type, nodeParams);
			assert(node!=nullptr);
			if (node!=nullptr)
			{
//				Events e = p_AddNode(node);
//				events.insert(end(events), begin(e), end(e));
				addNode(node);
			}
			else
			{
				errors.push_back("Unanticipated error when creating node '"+nodeParams.name+"'.");
			}
		}
		for (auto& jPatchCord: json["patchcords"])
		{
			string jOutlet = jPatchCord["outlet"].asString();
			if (jOutlet.empty())
			{
				errors.push_back("patchcord element found with no 'outlet' element. Skipping this patch cord.");
			}
			string jInlet = jPatchCord["inlet"].asString();
			if (jInlet.empty())
			{
				errors.push_back("patchcord element found with no 'inlet' element. Skipping this patch cord.");
			}
			OutletPtr outlet = outletFromPath(jOutlet);
			if (outlet==nullptr)
			{
				errors.push_back("patchcord element found with an outlet that could not be found: '"+jOutlet+"'. Skipping this patchcord.");
			}
			InletPtr inlet = inletFromPath(jInlet);
			if (inlet==nullptr)
			{
				errors.push_back("patchcord element found with an inlet that could not be found: '"+jInlet+"'. Skipping this patchcord.");
			}
			
			if (inlet!=nullptr && outlet!=nullptr)
			{
//				Events e = p_Connect(outlet, inlet);
//				events.insert(end(events), begin(e), end(e));
//				assert(!e.empty());
				connect(outlet, inlet);
			}
		}
//		if (wasRunning)
//		{
//			start();
//		}
	}
//	p_Process(events);
	return errors.empty();
}


bool Pipeline::saveJson(string filePath) const
{
	Json::Value json;
	toJson(json);
	ofstream out(filePath, ofstream::out);
	if (!out.good())
	{
		hm_warning("Failed to open "+filePath+" for writing. Pipeline has not been saved.");
		return false;
	}
	Json::StyledStreamWriter writer;
	writer.write(out, json);
	if (out.fail())
	{
		hm_warning("Error when writing to "+filePath+". Pipeline has not been saved.");
		return false;
	}
	return true;
}


namespace hm
{
	std::ostream& operator<<(std::ostream& out, Pipeline const& rhs)
	{
		SharedLock lock(rhs.mPipelineMutex);
		out << "[Pipeline: "
		<< (rhs.mIsRunning? "running" : "stopped")
		<< "\nThread running: "<<!rhs.mThread->get_thread_info()->done
		<< "\nNodes: [";
		for (int i=0; i<rhs.mNodes.size(); i++)
		{
			out << indent("\n"+rhs.mNodes[i]->toString() + (i < rhs.mNodes.size()-1? "," : ""));
		}
		out << "\n],\nPatchcords: [";
		for (auto it=begin(rhs.mPatchCords); it!=end(rhs.mPatchCords); )
		{
			out << indent((**it).toString());
			++it;
			out << indent(it!=end(rhs.mPatchCords)? ",\n" : "");
		}
		return out << "\n]";
	}
}



