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
	// Create a temporary instance, grab the params.
	NodePtr instance = nodeInfo.creationFunction(Node::Params());
	mNodeParams[nodeInfo.className] = instance->exportParams();
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

Node::Params FactoryNode::createParams(std::string const& className)
{
	assert(hasNodeType(className) == (mNodeParams.count(className)==1));
	if (hasNodeType(className))
	{
		return mNodeParams.at(className);
	}
	else
	{
		return Node::Params();
	}
}

NodePtr FactoryNode::create(std::string className, Node::Params params)
{
    cleanUp();
	mNodeFunctionsCalled = true;
	if (!hasNodeType(className))
	{
		return nullptr;
	}
	else
	{
		NodePtr node = mNodeInfos.at(className).creationFunction(params);
        assert(mCreatedNodes.count(node.get()) == 0);
        mCreatedNodes[node.get()] = std::weak_ptr<Node>(node);
        return node;
	}
}

NodePtr FactoryNode::getNodePtr(Node* existingNode) const
{
    if (mCreatedNodes.count(existingNode)==0)
    {
        return nullptr;
    }
    return mCreatedNodes.at(existingNode).lock();
    
}

void FactoryNode::cleanUp()
{
    for (auto it=mCreatedNodes.begin(); it!=mCreatedNodes.end(); )
    {
        if (it->second.expired())
        {
            it = mCreatedNodes.erase(it);
        }
        else
        {
            ++it;
        }
    }
}





