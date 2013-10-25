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

Outlet::Outlet(Types types, Node& node, string const& name, string const& helpText)
: mTypes(types)
, mName(name)
, mNode(&node)
, mHelpText(helpText)
{
}

void Outlet::detachOwnerNode()
{
	mNode = nullptr;
}

//std::string Outlet::nodeName() const
//{
//	return node != nullptr? node->type() : "(No node set)";
//}

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
	// Add this node to the data's history list
	{
		deque<std::string>& history = data.asDataType()->nodeHistory;
		if (mNode != nullptr)
		{
			history.push_front(mNode->name());
		}
		else
		{
			history.push_front("(unknown)");
		}
	}
	hm_debug("New data at outlet ("+nodeName()+"): "+data.toString());
	for (auto outRefIt=mOutputs.begin(); outRefIt!=mOutputs.end(); )
	{
		InletPtr out = outRefIt->lock();
		// check the inlet still exists. if it doesn't then remove the connection
		if (out == nullptr)
		{
			outRefIt = mOutputs.erase(outRefIt);
		}
		else
		{
			// Only send data to inlets that support it
			if (out->types() & data.type())
			{
				out->provideNewData(data);
			}
			++outRefIt;
		}
	}
}

