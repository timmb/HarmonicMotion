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
, mData(DataNull())
, mTimestamp(-42.)
{}

//Data::Data(Type type, double timestamp)
//: mType(type)
//, mData(createVariant(type))
//, mTimestamp(timestamp)
//{}

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

DataType* Data::asDataType()
{
	switch (mType)
	{
		case UNDEFINED:
			return &boost::get<DataNull&>(mData);
		case VALUE:
			return &boost::get<Value&>(mData);
		case POINT3D:
			return &boost::get<Point3d&>(mData);
		case SKELETON3D:
			return &boost::get<Skeleton3d&>(mData);
		case SCENE3D:
			return &boost::get<Scene3d&>(mData);
		default:
			assert(false);
			return nullptr;
	}
}


DataType const* Data::asDataType() const
{
	switch (mType)
	{
		case UNDEFINED:
			return &boost::get<DataNull const&>(mData);;
		case VALUE:
			return &boost::get<Value const&>(mData);
		case POINT3D:
			return &boost::get<Point3d const&>(mData);
		case SKELETON3D:
			return &boost::get<Skeleton3d const&>(mData);
		case SCENE3D:
			return &boost::get<Scene3d const&>(mData);
		default:
			assert(false);
			return nullptr;
	}
}

bool Data::isNull() const
{
	return mType == UNDEFINED;
}

DataNull& Data::asNull()
{
	return boost::get<DataNull&>(mData);
}

DataNull const& Data::asNull() const
{
	return boost::get<DataNull const&>(mData);
}

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
		return out << *rhs.asDataType();
	}
}

std::string Data::toString() const
{
	return (std::stringstream() << *this).str();
}

//Data::Variant Data::createVariant(Type type)
//{
//	switch (type)
//	{
//		case UNDEFINED:
//			return DataNull();
//		case VALUE:
//			return Value();
//		case POINT3D:
//			return Point3d();
//		case SKELETON3D:
//			return Skeleton3d();
//		case SCENE3D:
//			return Scene3d();
//		default:
//			assert(false);
//			return Value();
//	}
//}
