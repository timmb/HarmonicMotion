//
//  FactoryNode.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 05/02/2014.
//  Copyright (c) 2014 Tim Murray-Browne. All rights reserved.
//

#include "FactoryNode.h"

using namespace hm;

FactoryNode::Ptr FactoryNode::instance()
{
	static FactoryNode::Ptr inst(new FactoryNode);
	return inst;
}

void FactoryNode::registerNodeType(NodeInfo const& nodeInfo)
{
	if (hasNodeType(nodeInfo.className))
	{
		hm_error("Attempted to register the same node twice");
		assert(!hasNodeType(nodeInfo.className));
		throw std::runtime_error("Node "+nodeInfo.className+" has already been regstered with FactoryNode.");
	}
	mNodeInfos.emplace(nodeInfo.className, nodeInfo);
	hm_info("Node type "+nodeInfo.className+" registered.");
}

bool FactoryNode::hasNodeType(std::string className) const
{
	return mNodeInfos.count(className)>0;
}

NodePtr FactoryNode::create(std::string className, Node::Params params) const
{
	if (!hasNodeType(className))
	{
		return nullptr;
	}
	else
	{
		return mNodeInfos.at(className).creationFunction(params);
	}
}
