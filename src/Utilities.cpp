//
//  Utilities.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 06/02/2014.
//
//

#include "Utilities.h"

namespace hm
{
	QString str(std::string utf8String)
	{
		return QString::fromUtf8(utf8String.c_str());
	}
}