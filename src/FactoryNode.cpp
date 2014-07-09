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
		hm_error("Node type "+nodeInfo.type+" was registered after creation/" "access functions were called. Nodes must all be registered earlier."
			" (Probably a bug)");
		assert(!mNodeFunctionsCalled);
		return;
	}
	if (hasNodeType(nodeInfo.type))
	{
		hm_error("Attempted to register the same node twice");
		assert(!hasNodeType(nodeInfo.type));
		throw std::runtime_error("Node "+nodeInfo.type+" has already been regstered with FactoryNode.");
	}
	mNodeInfos.emplace(nodeInfo.type, nodeInfo);
	hm_info("Node type "+nodeInfo.type+" registered.");
}

bool FactoryNode::hasNodeType(std::string nodeTypeName) const
{
	return mNodeInfos.count(nodeTypeName)>0;
}

bool FactoryNode::hasNodeClass(std::string nodeClassName) const
{
	for (auto const& p: mNodeInfos)
	{
		if (p.second.className == nodeClassName)
		{
			return true;
		}
	}
	return false;
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

Node::Params FactoryNode::createParams(std::string const& nodeTypeName)
{
	if (hasNodeType(nodeTypeName))
	{
		return mNodeInfos.at(nodeTypeName).params;
	}
	else
	{
		return Node::Params();
	}
}

NodePtr FactoryNode::create(std::string nodeTypeName, Node::Params params)
{
    cleanUp();
	mNodeFunctionsCalled = true;
	if (!hasNodeType(nodeTypeName))
	{
		return nullptr;
	}
	else
	{
		NodePtr node = mNodeInfos.at(nodeTypeName).creationFunction(params);
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





