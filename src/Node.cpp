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

namespace hm
{
	std::ostream& operator<<(std::ostream& out, Node const& node)
	{
		return out
		<< "Node "<<node.name()<<" ("<<node.type()<<")"
		<<" inlets:"<<node.numInlets()
		<<" outlets:"<<node.numOutlets();
	}
}

using namespace hm;
using namespace std;

set<string> Node::sNamesInUse;
boost::mutex Node::sNamesInUseMutex;

Node::Node(Params params, string const& className)
: mClassName(className)
, mIsEnabled(true)
, mIsProcessing(false)
, mHasStartEverBeenCalled(false)
{
	assert(mClassName != "");
	setNodeParams(params);
	hm_debug("Node constructed: "+className);
}

Node::~Node()
{
	for (InletPtr inlet: mInlets)
	{
		inlet->detachOwnerNode();
	}
}

std::string Node::path() const
{
	return '/'+name();
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


void Node::stopProcessing()
{
	assert(mIsProcessing);
	hm_debug("Processing stopped on Node "+name());
}


void Node::updateParameters()
{
	boost::shared_lock<boost::shared_mutex> lock(mParametersMutex);
	for (ParameterPtr p: mParameters)
	{
		p->update();
	}
}

std::vector<ParameterPtr> Node::parameters()
{
	boost::shared_lock<boost::shared_mutex> lock(mParametersMutex);
	return mParameters;
}


InletPtr Node::inlet(int index)
{
	assert(0 <= index && index < mInlets.size());
	return mInlets[index];
}

OutletPtr Node::outlet(int index)
{
	assert(0 <= index && index < mOutlets.size());
	return mOutlets[index];
}

const InletPtr Node::inlet(int index) const 
{
	assert(0 <= index && index < mInlets.size());
	return mInlets[index];
}

const OutletPtr Node::outlet(int index) const
{
	assert(0 <= index && index < mOutlets.size());
	return mOutlets[index];
}

int Node::numInlets() const
{
	return mInlets.size();
}

int Node::numOutlets() const
{
	return mOutlets.size();
}

std::string Node::toString() const
{
	return (stringstream() << *this).str();
}

void Node::setName(std::string name)
{
	Params params = nodeParams();
	params.name = name;
	setNodeParams(params);
}


Node::Params Node::nodeParams() const
{
	boost::shared_lock<boost::shared_mutex> lock(mNodeParamsMutex);
	return mNodeParams;
}

void Node::setNodeParams(Params& params)
{
	boost::unique_lock<boost::shared_mutex> lock(mNodeParamsMutex);
	{ // validate name
		if (params.name=="")
			params.name = mClassName;
		// if we have a new name, ensure it is unique
		if (params.name != mNodeParams.name)
		{
			boost::unique_lock<boost::mutex> lock(sNamesInUseMutex);
			
			std::string requestedName = params.name;
			int count = 2;
			while (sNamesInUse.count(params.name)>0)
			{
				params.name = requestedName+' '+ci::toString(count);
				count++;
			}
			sNamesInUse.erase(mNodeParams.name);
			sNamesInUse.insert(params.name);
		}
	}
	mNodeParams = params;
}


InletPtr Node::createInlet(Types types, std::string const& name, std::string const& helpText)
{
	// For thread-safety, inlets may only be created before start() is called
	if (mHasStartEverBeenCalled)
	{
		hm_error("Inlets cannot be created after start() has been called.");
		assert(!mHasStartEverBeenCalled);
		// TODO: Replace with custom exception
		throw std::runtime_error("Inlets cannot be created after start() has been called.");
	}
	InletPtr inlet(new Inlet(types, *this, name, helpText));
	mInlets.push_back(inlet);
	return inlet;
}

OutletPtr Node::createOutlet(Types types, std::string const& name, std::string const& helpText)
{
	// For thread-safety, outlets may only be created before start() is called
	if (mHasStartEverBeenCalled)
	{
		hm_error("Outlets cannot be created after start() has been called.");
		assert(!mHasStartEverBeenCalled);
		// TODO: Replace with custom exception
		throw std::runtime_error("Outlets cannot be created after start() has been called.");
	}
	OutletPtr outlet(new Outlet(types, *this, name, helpText));
	mOutlets.push_back(outlet);
	return outlet;
}
