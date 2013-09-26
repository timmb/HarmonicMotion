//
//  Pipeline.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Pipeline.h"

//#include <XnOpenNI.h>
#include "cinder/Vector.h"
#include "cinder/Rect.h"
#include "cinder/Area.h"

int Pipeline::test()
{
	ci::Vec2f v(0.5, 1.5);
	v.x += 3;
	ci::Area a;
	a.offset(ci::Vec2i(50,50));
	return v.length();
//	return 10;
//	return a.getX1();
//	return 4;
}