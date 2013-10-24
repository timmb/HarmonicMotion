//
//  Common.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 21/10/2013.
//
//

#include "Common.h"
#include "cinder/Timer.h"

namespace hm
{
	
	ci::Timer programTimer(true);
	
	double elapsedTime()
	{
		return programTimer.getSeconds();
	}
	
	std::vector<Type> const& listOfTypes()
	{
		static const std::vector<Type> types =
		{
			VALUE,
			POINT3D,
			SKELETON3D,
			SCENE3D,
			IMAGE3D
		};
		return types;
	}
	
}
