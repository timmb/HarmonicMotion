//
//  Let.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 28/02/2014.
//
//

#include "Let.h"
#include "FactoryNode.h"

using namespace hm;


std::weak_ptr<Node> Let::node() const
{
    return FactoryNode::instance()->getNodePtr(mNode);
}


void Let::detachOwnerNode()
{
	mNode = nullptr;
}