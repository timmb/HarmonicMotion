//
//  Value.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "Value.h"
#include <sstream>

namespace hm
{
	std::ostream& operator<<(std::ostream& out, Value const& rhs)
	{
		return out << rhs.value();
	}
}

using namespace hm;

Value::Value(double value)
: mValue(value)
{}

Value::Value(float value)
: mValue(value)
{}

std::string Value::toString() const
{
	return (std::stringstream() << *this).str();
}