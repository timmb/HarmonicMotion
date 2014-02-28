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
{
}


std::weak_ptr<Node> Let::node() const
{
    return FactoryNode::instance()->getNodePtr(mNode);
}


void Let::detachOwnerNode()
{
	mNode = nullptr;
}


std::string Let::path() const
{
	assert(mNode != nullptr);
	if (mNode==nullptr)
	{
		return "[detached]/"+name();
	}
	else
	{
		return mNode->path() + '/' + name();
	}
}