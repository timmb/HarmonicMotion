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


Let::Let(Types types, Node& owner, std::string const& name, std::string const& helpText)
: mNode(&owner)
, mTypes(types)
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


std::string Let::path() const
{
	NodePtr n = node().lock();
	if (n==nullptr)
	{
		return "[detached]/"+name();
	}
	else
	{
		return n->path() + '/' + name();
	}
}