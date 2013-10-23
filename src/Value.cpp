//
//  Value.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "Value.h"
#include <sstream>

using namespace hm;

std::ostream& Value::printTo(std::ostream& out) const
{
	return out << mValue;
}

Value::Value(double value)
: mValue(value)
{}

Value::Value(float value)
: mValue(value)
{}

