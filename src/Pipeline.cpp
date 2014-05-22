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
#include <boost/thread/lock_guard.hpp>


using namespace hm;
using namespace std;
typedef boost::lock_guard<boost::mutex> Lock;

Pipeline::Pipeline()
: mIsRunning(false)
{
	hm_debug("Pipeline constructed");
}

Pipeline::~Pipeline()
{
}

std::string Pipeline::toString() const
{
	stringstream ss;
	ss << *this;
	return ss.str();
}

std::vector<NodePtr> const& Pipeline::nodes() const
{
	return mNodes;
}

void Pipeline::addNode(NodePtr node)
{
	{
		Lock lock(mMutex);
		mNodes.push_back(node);
		hm_debug("Added node "+node->toString());
		if (isRunning() && !node->isProcessing())
		{
			node->startProcessing();
		}
	}
	for (Listener* listener: mListeners)
	{
		listener->nodeAdded(node);
	}
}

void Pipeline::removeNode(NodePtr node)
{
	auto it = std::find(mNodes.begin(), mNodes.end(), node);
	if (it != mNodes.end())
	{
		Lock lock(mMutex);
		if (isRunning())
		{
			assert((**it).isProcessing());
			{
				(**it).stopProcessing();
			}
		}
		mNodes.erase(it);
	}
	/// Check invariants
	for (OutletPtr outlet: node->outlets())
	{
		assert(outlet->numInlets()==0);
	}
	assert(patchCordInvariant());
	hm_debug("Removed node "+node->toString());
	for (Listener* listener: mListeners)
	{
		listener->nodeRemoved(node);
	}
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
			{
				Lock lock(mMutex);
				for (NodePtr node: mNodes)
				{
					node->stepProcessing();
				}
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
	}));
}

void Pipeline::stop()
{
	mIsRunning = false;
}


bool Pipeline::patchCordInvariant() const
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
		for (NodePtr node: mNodes)
		{
			for (InletPtr inlet: node->inlets())
			{
				inletNumConnectionsInNodes[inlet]++;
			}
		}
		std::map<InletPtr, int> inletNumConnectionsInPatchCords;
		for (PatchCordPtr p: mPatchCords)
		{
			inletNumConnectionsInPatchCords[p->inlet()]++;
		}
		if (inletNumConnectionsInNodes != inletNumConnectionsInPatchCords)
		{
			return true;
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
	assert(patchCordInvariant());
	bool connectionFail(false);
	std::string failReason;
	if (isConnected(outlet, inlet))
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
		return false;
	}
	
	PatchCordPtr p(new PatchCord(outlet, inlet));
	{
		Lock lock(mMutex);
		outlet->addPatchCord(p);
		inlet->incrementNumConnections();
		mPatchCords.push_back(p);
	}
	assert(patchCordInvariant());
	hm_debug("Connected "+outlet->toString()+" and "+inlet->toString());
	for (Listener* listener: mListeners)
	{
		listener->patchCordAdded(outlet, inlet);
	}
	return true;
}


bool Pipeline::disconnect(OutletPtr outlet, InletPtr inlet)
{
	auto it=find_if(mPatchCords.begin(), mPatchCords.end(), [&](PatchCordPtr p) {
		return p->outlet() == outlet && p->inlet() == inlet;
	});
	if (it==mPatchCords.end())
	{
		hm_info("Unable to disconnect "+outlet->toString()+" from "+inlet->toString()+" as they are not connected.");
		return false;
	}
	disconnect(*it);
	return true;
}


void Pipeline::disconnect(PatchCordPtr p)
{
	assert(patchCordInvariant());
	{
		Lock lock(mMutex);
		p->outlet()->removePatchCord(p);
		p->inlet()->decrementNumConnections();
		mPatchCords.erase(find(mPatchCords.begin(), mPatchCords.end(), p));
	}
	assert(patchCordInvariant());
	hm_debug("Disconnected "+p->outlet()->toString()+" and "+p->inlet()->toString());
	for (Listener* listener: mListeners)
	{
		listener->patchCordRemoved(p->outlet(), p->inlet());
	}
}


bool Pipeline::isConnected(OutletPtr outlet, InletPtr inlet) const
{
	// outlet is connected to inlet if there exists a patch cord containing
	// them both.
	return find_if(mPatchCords.begin(), mPatchCords.end(), [&](PatchCordPtr p) {
		return p->outlet() == outlet && p->inlet() == inlet;
	}) != mPatchCords.end();
}


void Pipeline::addListener(Listener* listener)
{
	Lock lock(mMutex);
	
	mListeners.push_back(listener);
}

bool Pipeline::removeListener(Listener* listener)
{
	Lock lock(mMutex);
	
	auto it = std::find(mListeners.begin(), mListeners.end(), listener);
	if (it==mListeners.end())
	{
		return false;
	}
	else
	{
		mListeners.erase(it);
		return true;
	}
}


NodePtr Pipeline::nodeFromPath(string path)
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


vector<string> tokenizePath(string path)
{
	using namespace boost;
	tokenizer<char_separator<char>> tokens(path, char_separator<char>("/"));
	std::vector<std::string> splitPath;
	copy(tokens.begin(), tokens.end(), back_inserter(splitPath));
	return splitPath;
}


OutletPtr Pipeline::outletFromPath(std::string path)
{
	std::vector<std::string> splitPath = tokenizePath(path);
	if (splitPath.size() < 2)
	{
		hm_info("Path '"+path+"' does not contain both a node name and outlet name.");
		return nullptr;
	}
	NodePtr node = nodeFromPath(splitPath[0]);
	if (node)
	{
		for (OutletPtr outlet: node->outlets())
		{
			if (outlet->name() == splitPath[1])
			{
				return outlet;
			}
		}
		hm_info("Node "+node->name()+" of type "+node->type()+" does not have "
				"an outlet named "+splitPath[1]);
		return nullptr;
	}
	else
	{
		hm_info("No Node found with name "+path);
		return nullptr;
	}
}


InletPtr Pipeline::inletFromPath(std::string path)
{
	std::vector<std::string> splitPath = tokenizePath(path);
	if (splitPath.size() < 2)
	{
		hm_info("Path '"+path+"' does not contain both a node name and inlet name.");
		return nullptr;
	}
	NodePtr node = nodeFromPath(splitPath[0]);
	if (node)
	{
		for (InletPtr inlet: node->inlets())
		{
			if (inlet->name() == splitPath[1])
			{
				return inlet;
			}
		}
		hm_info("Node "+node->name()+" of type "+node->type()+" does not have "
				"an inlet named "+splitPath[1]);
		return nullptr;
	}
	else
	{
		hm_info("No Node found with name "+path);
		return nullptr;
	}
}


void Pipeline::toJson(Json::Value& json) const
{
	Lock lock(mMutex);
	
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
	hm_debug("Pipeline::clear(): Erasing all patchcords");
	while (!mPatchCords.empty())
	{
		disconnect(mPatchCords.back());
	}
	hm_debug("Pipeline::clear(): Erasing all nodes");
	while (!mNodes.empty())
	{
		removeNode(mNodes.back());
	}
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
	
	bool wasRunning = isRunning();
	if (wasRunning)
	{
		stop();
	}
	clear();
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
			
			bool connectSuccess = false;
			if (inlet!=nullptr && outlet!=nullptr)
			{
				connectSuccess = connect(outlet, inlet);
			}
			assert(connectSuccess);
		}
	if (wasRunning)
	{
		start();
	}
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
		out << "[Pipeline: "
		<< (rhs.mIsRunning? "running" : "stopped")
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



