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
#include "Skeleton3d.h"
#include "Scene3d.h"
#include "Image2d.h"


using namespace hm;
using namespace std;

std::ostream& Value::printTo(std::ostream& out) const
{
	return out << value;
}

Value::Value(double value_, double timestamp, int id, SceneMetaPtr sceneMeta)
: Base1dData(timestamp, id, sceneMeta)
, value(value_)
{}

Value::Value(float value_, double timestamp, int id, SceneMetaPtr sceneMeta)
: Base1dData(timestamp, id, sceneMeta)
, value(value_)
{}


//Value Value::operator+(Value const& rhs) const
//{
//	return Value(value + rhs.value, max(timestamp, rhs.timestamp), sceneMeta);
//}
//
//Value Value::operator-(Value const& rhs) const
//{
//	return Value(value - rhs.value, max(timestamp, rhs.timestamp), sceneMeta);
//}
//
//Value Value::operator*(Value const& rhs) const
//{
//	return Value(value * rhs.value, max(timestamp, rhs.timestamp), sceneMeta);
//}
//
//Value Value::operator/(Value const& rhs) const
//{
//	return Value(value / rhs.value, max(timestamp, rhs.timestamp), sceneMeta);
//}


Value& Value::operator+=(Value const& rhs)
{
	value += rhs.value;
	timestamp = max(timestamp, rhs.timestamp);
	return *this;
}

Value& Value::operator-=(Value const& rhs)
{
	value -= rhs.value;
	timestamp = max(timestamp, rhs.timestamp);
	return *this;
}

Value& Value::operator*=(Value const& rhs)
{
	value *= rhs.value;
	timestamp = max(timestamp, rhs.timestamp);
	return *this;
}

Value& Value::operator/=(Value const& rhs)
{
	value /= rhs.value;
	timestamp = max(timestamp, rhs.timestamp);
	return *this;
}


//Point3d Value::operator*(Point3d const& rhs) const
//{
//	return rhs * *this;
//}
//
//Point3d Value::operator/(Point3d const& rhs) const
//{
//	return rhs * (1. / *this);
//}

Skeleton3d Value::operator*(Skeleton3d const& rhs) const
{
	return rhs * *this;
}

Skeleton3d Value::operator/(Skeleton3d const& rhs) const
{
	return rhs * (1. / *this);
}

Scene3d Value::operator*(Scene3d const& rhs) const
{
	return rhs * *this;
}

Scene3d Value::operator/(Scene3d const& rhs) const
{
	return rhs * (1. / *this);
}

//Image2d Value::operator+(Image2d const& rhs) const
//{
//	return rhs + *this;
//}
//
//Image2d Value::operator-(Image2d const& rhs) const
//{
//	return rhs - *this;
//}
//
//Image2d Value::operator*(Image2d const& rhs) const
//{
//	return rhs * *this;
//}
//
//Image2d Value::operator/(Image2d const& rhs) const
//{
//	return rhs / *this;
//}


//Point3d Value::operator*(Point3d const& rhs) const
//{
//	return rhs * value();
//}
//
//Point3d Value::operator/(Point3d const& rhs) const
//{
//	return rhs * (1. / value());
//}

