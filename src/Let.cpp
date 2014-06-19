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


Let::Let(Types types, Node& owner, int index, std::string const& name, std::string const& helpText)
: mTypes(types)
, mNode(&owner)
, mIndex(index)
, mName(name)
, mHelpText(helpText)
, mIsDetached(false)
{
}


std::weak_ptr<Node> Let::node() const
{
    return FactoryNode::instance()->getNodePtr(mNode);
}


void Let::detachOwnerNode()
{
	mIsDetached = true;
	mNode = nullptr;
}


