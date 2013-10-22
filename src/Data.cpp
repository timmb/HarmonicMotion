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

Data::Data(Skeleton3d& x, double timestamp)
: mType(SKELETON3D)
, mData(x)
, mTimestamp(timestamp)
{}

Data::Data(Scene3d& x, double timestamp)
: mType(SCENE3D)
, mData(x)
, mTimestamp(timestamp)
{}

bool Data::isValue() const
{
	return mType == VALUE;
}

Value const& Data::asValue() const
{
	assert(isValue());
	return boost::get<Value const&>(mData);
}

Value& Data::asValue()
{
	assert(isValue());
	return boost::get<Value&>(mData);
}

bool Data::isPoint3d() const
{
	return mType == POINT3D;
}

Point3d const& Data::asPoint3d() const
{
	assert(isPoint3d());
	return boost::get<Point3d const&>(mData);
}

Point3d& Data::asPoint3d()
{
	assert(isPoint3d());
	return boost::get<Point3d&>(mData);
}


bool Data::isSkeleton3d() const
{
	return mType == SKELETON3D;
}

Skeleton3d const& Data::asSkeleton3d() const
{
	assert(isSkeleton3d());
	return boost::get<Skeleton3d const&>(mData);
}

Skeleton3d& Data::asSkeleton3d()
{
	assert(isSkeleton3d());
	return boost::get<Skeleton3d&>(mData);
}


bool Data::isScene3d() const
{
	return mType == SCENE3D;
}

Scene3d const& Data::asScene3d() const
{
	assert(isScene3d());
	return boost::get<Scene3d const&>(mData);
}

Scene3d& Data::asScene3d()
{
	assert(isScene3d());
	return boost::get<Scene3d&>(mData);
}

std::string Data::toString() const
{
	std::stringstream ss;
	switch (mType)
	{
		case VALUE:
			ss << asValue();
			break;
		case POINT3D:
			ss << asPoint3d();
			break;
		case SKELETON3D:
			ss << asSkeleton3d();
			break;
		case SCENE3D:
			ss << asScene3d();
			break;
		default:
			ss << "Unable to convert datatype into string";
			assert(false);
			break;
	}
	return ss.str();
}

