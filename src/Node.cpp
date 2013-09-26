//
//  Node.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Node.h"

using namespace hm;
using namespace std;

Node::Node(string const& className)
: mClassName(className)
{
	assert(mClassName != "");
}

