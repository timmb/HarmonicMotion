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
#include "FactoryNode.h"
#include "PatchCord.h"
#include <algorithm>

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

//bool Outlet::connect(InletPtr inlet)
//{
//	if ((inlet->types() & types()) == 0)
//	{
//		return false;
//	}
//	mOutputs.push_back(inlet);
//	inlet->incrementNumConnections();
//	return true;
//}

void Outlet::addPatchCord(PatchCordPtr patchCord)
{
	bool isValid = std::find(mPatchCords.begin(), mPatchCords.end(), patchCord) != mPatchCords.end()
		&& patchCord->outlet().get() == this;
	assert(isValid);
	if (isValid)
	{
		mPatchCords.push_back(patchCord);
	}
}

void Outlet::removePatchCord(PatchCordPtr patchCord)
{
	auto it = std::find(mPatchCords.begin(), mPatchCords.end(), patchCord);
	bool isValid = it != mPatchCords.end();
	assert(isValid);
	if (isValid)
	{
		mPatchCords.erase(it);
	}
}


void Outlet::outputNewData(Data& data)
{
	bool isDataTypeValid = mTypes & data.type();
	assert(isDataTypeValid);
	if (isDataTypeValid)
	{
		// Add this node to the data's history list
		deque<std::string>& history = data.asDataType()->nodeHistory;
		if (mNode != nullptr)
		{
			history.push_front(mNode->name());
		}
		else
		{
			history.push_front("(unknown)");
		}
		hm_debug("New data at outlet ("+nodeName()+"): "+data.toString());
		for (PatchCordPtr cord: mPatchCords)
		{
			assert(data.type() & cord->inlet()->types());
			cord->inlet()->provideNewData(data);
		}
	}
//	for (auto outRefIt=mOutputs.begin(); outRefIt!=mOutputs.end(); )
//	{
//		InletPtr out = outRefIt->lock();
//		// check the inlet still exists. if it doesn't then remove the connection
//		if (out == nullptr)
//		{
//			outRefIt = mOutputs.erase(outRefIt);
//		}
//		else
//		{
//			// Only send data to inlets that support it
//			if (out->types() & data.type())
//			{
//				out->provideNewData(data);
//			}
//			++outRefIt;
//		}
//	}
}


std::weak_ptr<Node> Outlet::node() const
{
    return FactoryNode::instance()->getNodePtr(mNode);
}


bool Outlet::isConnectedTo(InletPtr inlet) const
{
	return std::any_of(mPatchCords.begin(), mPatchCords.end(), [&](PatchCordPtr p) {
		return p->inlet()==inlet;
	});
//    for (auto weakInletPtr: mOutputs)
//    {
//        InletPtr i = weakInletPtr.lock();
//        assert(i != nullptr);
//        if (i==inlet)
//        {
//            return true;
//        }
//    }
//    return false;
}


//void Outlet::disconnect(InletPtr inlet)
//{
//    std::weak_ptr<Inlet> weakPtr(inlet);
//    for (auto it=mOutputs.begin(); it!=mOutputs.end(); )
//    {
//        InletPtr o = it->lock();
//        assert(o != nullptr);
//        if (o == inlet)
//        {
//            it = mOutputs.erase(it);
//            return;
//        }
//        else
//        {
//            ++it;
//        }
//    }
//    // If we reach here then we weren't connected to inlet.
//    assert(false);
//}





