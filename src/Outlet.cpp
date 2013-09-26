//
//  Outlet.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Outlet.h"

using namespace std;
using namespace hm;

Outlet::Outlet(Type type, string const& name, string const& helpText)
: mType(type)
, mName(name)
, mHelpText(helpText)
, mNumConnections(0)
{
	
}

void Outlet::incrementNumConnections()
{
	mNumConnections++;
}

void Outlet::decrementNumConnections()
{
	assert(mNumConnections>0);
	mNumConnections--;
}

bool Outlet::hasConnections() const
{
	return mNumConnections;
}

