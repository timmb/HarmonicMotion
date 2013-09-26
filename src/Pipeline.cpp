//
//  Pipeline.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Pipeline.h"

//#include <XnOpenNI.h>
#include "cinder/Area.h"

int Pipeline::test()
{
	ci::Area a;
	a.offset(ci::Vec2i(5,5));
	return a.getX1();
}