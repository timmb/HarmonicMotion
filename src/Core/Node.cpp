//
//  Node.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Node.h"
#include "Inlet.h"
#include "Outlet.h"
#include "cinder/Utilities.h"
#include "Parameter.h"
#include "Common.h"
#include "FactoryNode.h"
#include "Pipeline.h"

namespace hm
{
	std::ostream& operator<<(std::ostream& out, Node const& node)
	{
		out	<< "Node "<<node.name()<<" ("<<node.type()<<")"
		<< "\nParameters:";
		for (auto p: node.parameters())
		{
			out << indent("\n" + p->toString());
		}
		out << "\nInlets:";
		for (InletPtr inlet: node.inlets())
		{
			out << indent("\n" + inlet->toString());
		}
		out << "\nOutlets:";
		for (OutletPtr outlet: node.outlets())
		{
			out << indent("\n" + outlet->toString());
		}
		return out;
	}
}

using namespace hm;
using namespace std;

typedef boost::shared_lock<boost::shared_mutex> SharedLock;
typedef boost::unique_lock<boost::shared_mutex> UniqueLock;

Node::Node(Params params, string className_, string type_, string description_)
: mPipeline(nullptr)
, mIsEnabled(true)
, mIsProcessing(false)
, mHasStartEverBeenCalled(false)
, mClassName(className_)
, mType(type_)
, mDescription(description_)
{
	assert(mClassName != "");
	// if no node name has been set, default it to the class name
	if (params.name == "")
	{
		params.name = type();
	}
	setNodeParams(params);
	hm_debug("Node constructed: "+className_);
	// initial value
	mHaveAllCharacteristicChangesBeenReported.test_and_set();
}

Node::~Node()
{
	hm_debug("Destroying node "+toString());
	if (isProcessing())
	{
		stopProcessing();
	}
	for (InletPtr inlet: mInlets)
	{
		inlet->detachOwnerNode();
		// Inlets should have had all their patch cords removed by the pipeline
		assert(inlet->numConnections()==0);
//		// Inlets should no longer be known by any third parties
//		assert(inlet.use_count()==1);
	}
	for (OutletPtr outlet: mOutlets)
	{
		assert(outlet->numInlets()==0);
//		assert(outlet.use_count()==1);
	}
	{
		boost::unique_lock<boost::shared_mutex> lock(mParametersMutex);
		for (ParameterPtr parameter: mParameters)
		{
			parameter->detach();
		}
	}
}

std::string Node::path() const
{
	return '/'+name();
}

Node::Params Node::exportParams() const
{
	Params params = nodeParams();
	params.parameterInitialValues.clear();
	for (ParameterConstPtr p: parameters())
	{
		ParameterValueContainer v = p->toContainer();
//		cout << "parameter "<<p->name()<<": "<<v<<endl;
		params.parameterInitialValues[p->name()] = v;
	}
	return params;
}

void Node::setEnabled(bool isEnabled)
{
	mIsEnabled = isEnabled;
}

void Node::startProcessing()
{
	assert(!mIsProcessing);
	mIsProcessing = true;
	mHasStartEverBeenCalled = true;
	hm_debug("Processing started on Node "+name());
}


bool Node::stepProcessing()
{
	/// notify the pipeline if our characteristics have changed.
	return !mHaveAllCharacteristicChangesBeenReported.test_and_set();
}


void Node::stopProcessing()
{
	assert(mIsProcessing);
	mIsProcessing = false;
	hm_debug("Processing stopped on Node "+name());
}


void Node::updateParameters()
{
	SharedLock lock(mParametersMutex);
	for (ParameterPtr p: mParameters)
	{
		p->update();
	}
}

std::vector<ParameterPtr> Node::parameters()
{
	SharedLock lock(mParametersMutex);
	return mParameters;
}


std::vector<ParameterConstPtr> Node::parameters() const
{
	std::vector<ParameterConstPtr> parameters;
	SharedLock lock(mParametersMutex);
	parameters.assign(mParameters.size(), nullptr);
	std::copy(mParameters.begin(), mParameters.end(), parameters.begin());
	return parameters;
}


InletPtr Node::inlet(int index) const
{
	SharedLock lock(mCharacteristicsMutex);
	assert(mInlets[index]->node().lock() != nullptr);
	return (0 <= index && index < mInlets.size())? mInlets[index] : nullptr;
}

vector<InletPtr> Node::inlets() const
{
	SharedLock lock(mCharacteristicsMutex);
	return mInlets;
}

OutletPtr Node::outlet(int index) const
{
	SharedLock lock(mCharacteristicsMutex);
	assert(mOutlets[index]->node().lock() != nullptr);
	return (0 <= index && index < mOutlets.size())? mOutlets[index] : nullptr;
}

vector<OutletPtr> Node::outlets() const
{
	SharedLock lock(mCharacteristicsMutex);
	return mOutlets;
}

int Node::numInlets() const
{
	SharedLock lock(mCharacteristicsMutex);
	return (int) mInlets.size();
}

int Node::numOutlets() const
{
	SharedLock lock(mCharacteristicsMutex);
	return (int) mOutlets.size();
}

std::string Node::toString() const
{
	stringstream ss;
	ss << *this;
	return ss.str();
}

void Node::setName(std::string const& name)
{
	Params params = nodeParams();
	params.name = name;
	setNodeParams(params);
}


Node::Params Node::nodeParams() const
{
	SharedLock lock(mNodeParamsMutex);
	return mNodeParams;
}

void Node::setNodeParams(Params& params)
{
	{
		UniqueLock lock(mNodeParamsMutex);
		mNodeParams = params;
	}
	// NB it is important not to call Pipeline::ensureNameIsUnique while
	// any lock is on mNodeParamsMutex as Pipeline::ensureNameIsUnique may
	// subsequently call a function of this class that will attempt a unique
	// lock on mNodeParamsMutex.
	// TODO: This needs to be shared pointer to be safe
	if (Pipeline* p = pipeline())
	{
		NodePtr me = FactoryNode::instance()->getNodePtr(this);
		if (me)
		{
			p->ensureNameIsUnique(me);
		}
	}
}


InletPtr Node::createInlet(Types types, std::string const& name, std::string const& helpText)
{
	UniqueLock lock(mCharacteristicsMutex);
	// check name is unique
	if (any_of(mInlets.begin(), mInlets.end(), [name](InletPtr i) {	return i->name() == name;	}))
	{
		hm_error("Inlets must have unique names for a given node. '"
				 +name+"' has already been used.");
		return nullptr;
	}
	int index = (int) mInlets.size();
	
	InletPtr inlet(new Inlet(types, *this, index, name, helpText));
	mInlets.push_back(inlet);
	notifyCharacteristicsChanged();
	return inlet;
}

OutletPtr Node::createOutlet(Types types, std::string const& name, std::string const& helpText)
{
	UniqueLock lock(mCharacteristicsMutex);
	// check name is unique
	if (any_of(mOutlets.begin(), mOutlets.end(), [name](OutletPtr o) {	return o->name() == name;	}))
	{
		hm_error("Outlets must have unique names for a given node. '"
				 +name+"' has already been used.");
		return nullptr;
	}
	
	int index = (int) mOutlets.size();
	OutletPtr outlet(new Outlet(types, *this, index, name, helpText));
	outlet->mSelf = outlet;
	mOutlets.push_back(outlet);
	notifyCharacteristicsChanged();
	return outlet;
}

namespace hm
{
	Json::Value& operator<<(Json::Value& value, Node const& node)
	{
		value["type"] << node.type();
		value["settings"] << node.nodeParams();
		std::vector<ParameterConstPtr> parameters = node.parameters();
		// double check all parameters have unique names
		for (auto it=parameters.begin(); it!=parameters.end(); ++it)
		{
			for (auto jt=it+1; jt!=parameters.end(); ++jt)
			{
				assert((**it).name()!=(**jt).name());
			}
		}
		for (auto p: parameters)
		{
			p->toJson(value["parameters"][p->name()]);
		}
		return value;
	}
	
	bool operator>>(Json::Value const& value, Node& node)
	{
		vector<ParameterPtr> parameters = node.parameters();

		if (node.type() != value["type"].asString())
		{
			hm_error("Unable to load settings from JSON node of type "
					 + value["type"].asString() + " into Node of type "
					 + node.type());
			assert(node.type() == value["type"].asString());
			return false;
		}
		assert(!node.isProcessing());
		// assert all parameters have unique names
		for (auto it=parameters.begin(); it!=parameters.end(); ++it)
		{
			for (auto jt=it+1; jt!=parameters.end(); ++jt)
			{
				assert((**it).name()!=(**jt).name());
			}
		}

		bool success(true);
		
		{
			boost::lock_guard<boost::shared_mutex> lock(node.mNodeParamsMutex);
			success = success && (value["settings"] >> node.mNodeParams);
		}
		
		{
			Json::Value jParameters = value["parameters"];
			vector<string> jsonParameterNames = jParameters.getMemberNames();
			
			// Write in values of parameters from json file
			for (string name: jsonParameterNames)
			{
				// find parameter
				auto it = find_if(parameters.begin(), parameters.end(), [name](ParameterPtr p) {
					return p->name() == name;
				});
				// if we've found the parameter try to write in
				if (it != parameters.end()
					&& (**it).fromJson(jParameters[name]))
				{
					// remove from our list of parameters once found so that
					// we know which ones don't get updated
					parameters.erase(it);
				}
				else
				{
					hm_warning("Unrecognised parameter name "+name
							   +" for node "+node.name()+" in JSON file.");
					success = false;
				}
			}
			// Warn if we have any Parameters left over
			if (!parameters.empty())
			{
				string warning = "Values were not found in JSON file for the "
						   "following parameters of node "+node.name()
						   +": ";
				for (int i=0; i<parameters.size(); i++)
				{
					warning += parameters[i]->name() + (i<parameters.size()-1? ", ":"");
				}
				hm_warning(warning);
			}
		}
		return success;
	}
	
	Json::Value& operator<<(Json::Value& value, Node::Params const& params)
	{
		value["name"] = params.name;
		value["guiLocationX"] = params.guiLocationX;
		value["guiLocationY"] = params.guiLocationY;
		return value;
	}
	

	bool operator>>(Json::Value const& value, Node::Params& params)
	{
		if (!(value["settings"]["name"].isConvertibleTo(Json::stringValue)
			  && value["settings"]["guiLocationX"].isConvertibleTo(Json::intValue)
			  && value["settings"]["guiLocationY"].isConvertibleTo(Json::intValue)
			  ))
		{
			hm_debug("Error when loading \"params\" object from JSON.");
			return false;
		}
		params.guiLocationX = value["settings"]["guiLocationX"].asInt();
		params.guiLocationY = value["settings"]["guiLocationY"].asInt();
		if (value["parameters"].isObject())
		{
			vector<string> parameterNames = value["parameters"].getMemberNames();
			for (string const& s: parameterNames)
			{
				for (auto& pair: params.parameterInitialValues)
				{
					if (pair.first == s)
					{
						ParameterValueContainerSetter visitor(value["parameters"][s]);
						bool success = boost::apply_visitor(visitor, pair.second);
						if (!success)
						{
							hm_warning("Unrecognised parameter: "<<s);
						}
					}
				}
			}
		}
		
		// TODO: Check name is unique
		string name = value["settings"]["name"].asString();
		if (!name.empty())
		{
			params.name = name;
			return true;
		}
		else
		{
			return false;
		}
		
	}

}



bool Node::removeInlet(InletPtr inlet)
{
	UniqueLock lock(mCharacteristicsMutex);
	auto it = find(begin(mInlets), end(mInlets), inlet);
	if (it!=end(mInlets))
	{
		mInlets.erase(it);
		inlet->detachOwnerNode();
		notifyCharacteristicsChanged();
		return true;
	}
	return false;
}

bool Node::removeOutlet(OutletPtr outlet)
{
	UniqueLock lock(mCharacteristicsMutex);
	auto it = find(begin(mOutlets), end(mOutlets), outlet);
	if (it!=end(mOutlets))
	{
		mOutlets.erase(it);
		(**it).detachOwnerNode();
		notifyCharacteristicsChanged();
		return true;
	}
	return false;
}


void Node::setPipeline(Pipeline* pipeline)
{
	mPipeline = pipeline;
	if (mPipeline.load()==nullptr)
	{
		mDispatcher = nullptr;
	}
	else
	{
		mDispatcher = mPipeline.load()->dispatcher;
		assert(mDispatcher != nullptr);
	}
}


void Node::notifyCharacteristicsChanged()
{
	// don't notify of inlets being added, etc in the constructor
	if (mHasStartEverBeenCalled)
	{
		mHaveAllCharacteristicChangesBeenReported.clear();
//		NodePtr me = FactoryNode::instance()->getNodePtr(this);
//		EventDispatcherPtr dispatcher = mDispatcher;
//		if (dispatcher != nullptr && dispatcher != nullptr)
//		{
//			dispatcher->dispatch(EventPtr(new NodeCharacteristicsChangedEvent(me)));
//		}
	}
}



