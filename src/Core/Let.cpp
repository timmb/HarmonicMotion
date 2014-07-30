//
//  Let.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 28/02/2014.
//
//

#include "Let.h"
#include "FactoryNode.h"
#include "Type.h"

using namespace hm;
using namespace std;

Let::Let(Types types_, Node& owner, int index_, std::string const& name, std::string const& description, std::string const& className_)
: mTypes(types_)
, mNode(&owner)
, mIndex(index_)
, mName(name)
, mDescription(description)
, mIsDetached(false)
, mClassName(className_)
{
	assert(types_!=UNDEFINED);
	mHelpText = className_+" "+to_string(index()+1)+": "+name+" (";
	for (Type type: listOfTypes())
	{
		if (types_ & type)
		{
			mHelpText += to_string(type) + ", ";
		}
	}
	// Check if we need to remove final comma
	if (mHelpText.back() != '(')
	{
		assert(mHelpText.size()>=2);
		assert(mHelpText.substr(mHelpText.size()-2, mHelpText.size()) == ", ");
		mHelpText.resize(mHelpText.size() - 2);
	}
	mHelpText += ")\n"+description;
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


