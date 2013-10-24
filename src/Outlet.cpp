//
//  Outlet.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Outlet.h"
#include "Inlet.h"
#include "Node.h"

using namespace std;
using namespace hm;

Outlet::Outlet(Types types, string const& name, string const& helpText)
: mTypes(types)
, mName(name)
, mNode(nullptr)
, mHelpText(helpText)
{
	
}

std::string Outlet::nodeName() const
{
	return mNode? mNode->type() : "(No node set)";
}

bool Outlet::connect(InletPtr inlet)
{
	if ((inlet->types() & types()) == 0)
	{
		return false;
	}
	mOutputs.push_back(inlet);
	inlet->incrementNumConnections();
	return true;
}


void Outlet::outputNewData(Data& data)
{
	assert(mTypes & data.type());
	assert(mNode != nullptr);
	data.lastNode = mNode;
	hm_debug("New data at outlet ("+nodeName()+"): "+data.toString());
	for (InletPtr out: mOutputs)
	{
		// Only send data to inlets that support it
		if (out->types() & data.type())
		{
			out->provideNewData(data);
		}
	}
}

void Outlet::setNode(Node* node)
{
	mNode = node;
}