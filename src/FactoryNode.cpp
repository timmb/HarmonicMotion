//
//  FactoryNode.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 05/02/2014.
//  Copyright (c) 2014 Tim Murray-Browne. All rights reserved.
//

#include "FactoryNode.h"
#include <algorithm>

using namespace hm;

FactoryNode::FactoryNode()
: mNodeFunctionsCalled(false)
{}

FactoryNode::Ptr FactoryNode::instance()
{
	static FactoryNode::Ptr inst(new FactoryNode);
	return inst;
}

void FactoryNode::registerNodeType(NodeInfo const& nodeInfo)
{
	if (mNodeFunctionsCalled)
	{
		hm_error("Node type "+nodeInfo.className+" was registered after creation/" "access functions were called. Nodes must all be registered earlier."
			" (Probably a bug)");
		assert(!mNodeFunctionsCalled);
		return;
	}
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

std::vector<NodeInfo> FactoryNode::nodeTypes()
{
	mNodeFunctionsCalled = true;
	// copy node infos to vector
	using namespace std;
	vector<NodeInfo> v;
	v.reserve(mNodeInfos.size());
	transform(mNodeInfos.begin(), mNodeInfos.end(), back_inserter(v)
			  , [](pair<string, NodeInfo> p) { return p.second; });
	return v;
}

NodePtr FactoryNode::create(std::string className, Node::Params params)
{
	mNodeFunctionsCalled = true;
	if (!hasNodeType(className))
	{
		return nullptr;
	}
	else
	{
		return mNodeInfos.at(className).creationFunction(params);
	}
}
