//
//  Pipeline.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Pipeline.h"
#include "Node.h"
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
	boost::thread thread([this]()
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
		});
}

void Pipeline::stop()
{
	mIsRunning = false;
}



