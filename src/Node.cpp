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

using namespace hm;
using namespace std;

set<string> Node::sNamesInUse;
boost::mutex Node::sNamesInUseMutex;

Node::Node(Params params, string const& className)
: mClassName(className)
, mIsEnabled(false)
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

std::string Node::toString() const
{
	return (stringstream() << *this).str();
}

std::ostream& operator<<(std::ostream& out, Node const& node)
{
	return out
	<< "Node "<<node.name()<<" ("<<node.type()<<")"
	<<" inlets:"<<node.numInlets()
	<<" outlets:"<<node.numOutlets();
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
			mNodeParams.name = mClassName;
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
	InletPtr inlet(new Inlet(types, *this, name, helpText));
	mInlets.push_back(inlet);
	return inlet;
}

OutletPtr Node::createOutlet(Types types, std::string const& name, std::string const& helpText)
{
	OutletPtr outlet(new Outlet(types, *this, name, helpText));
	mOutlets.push_back(outlet);
	return outlet;
}
