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


using namespace hm;

Pipeline::Pipeline()
: mIsRunning(false)
{
	hm_debug("Pipeline constructed");
}

Pipeline::~Pipeline()
{
	for (NodePtr node: mNodes)
	{
		// See explanation at docnote for Node::disconnectAllCallbacks
		node->disconnectAllCallbacks();
	}
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
	for (auto p: mNodes)
	{
		p->start();
	}
}