//
//  Data.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 26/09/2013.
//
//

#include "Data.h"
#include <sstream>

using namespace hm;
using namespace std;


Data::Data()
: mType(UNDEFINED)
, mData(0.)
, mTimestamp(-42.)
{}

Data::Data(Value& value, double timestamp)
: mType(VALUE)
, mData(value)
, mTimestamp(timestamp)
{}

Data::Data(Point3d& x, double timestamp)
: mType(POINT3D)
, mData(x)
, mTimestamp(timestamp)
{}

bool Data::isValue() const
{
	return mType == VALUE;
}

Value const& Data::asValue() const
{
	return boost::get<Value const&>(mData);
}

Value& Data::asValue()
{
	return boost::get<Value&>(mData);
}

bool Data::isPoint3d() const
{
	return mType == POINT3D;
}

Point3d const& Data::asPoint3d() const
{
	return boost::get<Point3d const&>(mData);
}

Point3d& Data::asPoint3d()
{
	return boost::get<Point3d&>(mData);
}

std::string Data::toString() const
{
	std::stringstream ss;
	switch (mType)
	{
		case VALUE:
		case POINT3D:
			ss << asValue();
			break;
		default:
			ss << "Unable to convert datatype into string";
			assert(false);
			break;
	}
	return ss.str();
}

