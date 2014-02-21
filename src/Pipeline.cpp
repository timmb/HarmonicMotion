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
#include <algorithm>
#include <boost/thread.hpp>

using namespace hm;

Pipeline::Pipeline()
: mIsRunning(false)
{
	hm_debug("Pipeline constructed");
}

Pipeline::~Pipeline()
{
}

std::vector<NodePtr> const& Pipeline::nodes() const
{
	return mNodes;
}

void Pipeline::addNode(NodePtr node)
{
	mNodes.push_back(node);
	hm_debug("Added node "+node->toString());
}

void Pipeline::removeNode(NodePtr node)
{
	auto it = std::find(mNodes.begin(), mNodes.end(), node);
	if (it != mNodes.end())
	{
		mNodes.erase(it);
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
	mThread = std::unique_ptr<boost::thread>(new boost::thread([this]()
		{
			double timeOfLastUpdate = elapsedTime();
			while (mIsRunning)
			{
				for (NodePtr node: mNodes)
				{
					node->stepProcessing();
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
	outlet->addPatchCord(p);
	inlet->incrementNumConnections();
	mPatchCords.push_back(p);
	assert(patchCordInvariant());
	return true;
}


bool Pipeline::disconnect(OutletPtr outlet, InletPtr inlet)
{
	assert(patchCordInvariant());
	auto it=find_if(mPatchCords.begin(), mPatchCords.end(), [&](PatchCordPtr p) {
		return p->outlet() == outlet && p->inlet() == inlet;
	});
	if (it==mPatchCords.end())
	{
		hm_info("Unable to disconnect "+outlet->toString()+" from "+inlet->toString()+" as they are not connected.");
		return false;
	}
	outlet->removePatchCord(*it);
	mPatchCords.erase(it);
	assert(patchCordInvariant());
	return true;
}



