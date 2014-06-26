//
//  Common.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 21/10/2013.
//
//

#include "Common.h"
#include "cinder/Timer.h"
#include <sstream>
#include "Data.h"

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
			UNDEFINED,
			VALUE,
			POINT2D,
			POINT3D,
			SKELETON3D,
			SCENE3D,
			IMAGE2D,
			LIST_VALUE,
			LIST_POINT2D,
			LIST_POINT3D
		};
		return types;
	}
	
	std::string indent(std::string const& s, int indentAmount)
	{
		std::string indent(indentAmount, ' ');
		std::stringstream ss;
		for (char c: s)
		{
			ss << c;
			if (c=='\n')
			{
				ss << indent;
			}
		}
		return ss.str();
	}
	
	
	double getTimestamp(Data const& x)
	{
		return x.timestamp();
	}
	
	double getTimestamp(BaseData const& x)
	{
		return x.timestamp;
	}

	
}
