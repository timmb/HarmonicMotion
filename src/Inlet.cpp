//
//  Inlet.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Inlet.h"

using namespace hm;
using namespace std;

Inlet::Inlet(Type type, string const& name, string const& helpText)
: mType(type)
, mName(name)
, mHelpText(helpText)
{
	assert(type!=UNDEFINED);
}