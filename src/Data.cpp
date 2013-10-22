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
, mData()
, mTimestamp(-42.)
{}

Data::Data(Type type, double timestamp)
: mType(type)
, mData(createVariant(type))
, mTimestamp(timestamp)
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

namespace hm {
	std::ostream& operator<<(std::ostream& out, Data const& rhs)
	{
		switch (rhs.mType)
		{
			case VALUE:
				return out << rhs.asValue();
				break;
			case POINT3D:
				return out << rhs.asPoint3d();
				break;
			case SKELETON3D:
				return out << rhs.asSkeleton3d();
				break;
			case SCENE3D:
				return out << rhs.asScene3d();
				break;
			default:
				assert(false);
				return out << "(unrecognised type)";
		}
	}
}

std::string Data::toString() const
{
	return (std::stringstream() << *this).str();
}

Data::Variant Data::createVariant(Type type)
{
	switch (type)
	{
		case VALUE:
			return Variant(Value());
		case POINT3D:
			return Point3d();
		case SKELETON3D:
			return Skeleton3d();
		case SCENE3D:
			return Scene3d();
		default:
			assert(false);
			return Value();
	}
}