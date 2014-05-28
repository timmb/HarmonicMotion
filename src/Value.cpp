//
//  Value.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "Value.h"
#include "Point3d.h"
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


Value Value::operator+(Value const& rhs) const
{
	return Value(value() + rhs.value());
}

Value Value::operator-(Value const& rhs) const
{
	return Value(mValue - rhs.mValue);
}

Value Value::operator*(Value const& rhs) const
{
	return Value(value() * rhs.value());
}

Value Value::operator/(Value const& rhs) const
{
	return Value(value() / rhs.value());
}

//Point3d Value::operator*(Point3d const& rhs) const
//{
//	return rhs * value();
//}
//
//Point3d Value::operator/(Point3d const& rhs) const
//{
//	return rhs * (1. / value());
//}

