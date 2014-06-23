//
//  Point3d.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "Point3d.h"
#include "Value.h"
#include "Skeleton3d.h"
#include "Scene3d.h"
#include "cinder/gl/gl.h"
#include <algorithm>
#include "DataNull.h"


using namespace hm;
using namespace ci;
using namespace std;

Point3d::Point3d(Vec3f const& v, double timestamp, int id, SceneMetaPtr sceneMeta)
: Base3dData(timestamp, id, sceneMeta)
, value(v)
{}
	


Point3d::Point3d(float x, float y, float z, double timestamp, int id, SceneMetaPtr sceneMeta)
: Point3d(ci::Vec3f(x,y,z), timestamp, id, sceneMeta)
{
	
}

Point3d::Point3d(double timestamp, int id, SceneMetaPtr sceneMeta)
: Base3dData(timestamp, id, sceneMeta)
{
	
}

Point3d::Point3d(Value const& x, Value const& y, Value const& z)
: Base3dData(chooseTimestamp(x, y, z), chooseId(x, y, z), chooseSceneMeta(x, y, z))
, value(x.value, y.value, z.value)
{}

std::ostream& Point3d::printTo(std::ostream& out) const
{
	return out << value;
}

void Point3d::draw()
{
	ci::gl::drawSphere(value, 5);
}

// MARK: operators

//DataNull Point3d::operator+(DataNull const& rhs) const
//{
//	assert(rhs.type() != UNDEFINED);
//	return rhs;
//}
//
//DataNull Point3d::operator-(DataNull const& rhs) const
//{
//	assert(rhs.type() != UNDEFINED);
//	return rhs;
//}
//
//
//DataNull Point3d::operator*(DataNull const& rhs) const
//{
//	assert(rhs.type() != UNDEFINED);
//	return rhs;
//}
//
//
//DataNull Point3d::operator/(DataNull const& rhs) const
//{
//	assert(rhs.type() != UNDEFINED);
//	return rhs;
//}




//Point3d Point3d::Point3d::operator+(Point3d const& rhs) const
//{
//	return Point3d(value + rhs.value, max(timestamp, rhs.timestamp), sceneMeta);
//}
//
//Point3d Point3d::operator-(Point3d const& rhs) const
//{
//	return Point3d(value - rhs.value, max(timestamp, rhs.timestamp), sceneMeta);
//}
//
//Point3d Point3d::operator*(Point3d const& rhs) const
//{
//	return Point3d(value * rhs.value, max(timestamp, rhs.timestamp), sceneMeta);
//}
//
//Point3d Point3d::operator/(Point3d const& rhs) const
//{
//	return Point3d(value / rhs.value, max(timestamp, rhs.timestamp), sceneMeta);
//}
//
//Point3d& Point3d::operator+=(Point3d const& rhs)
//{
//	timestamp = max(timestamp, rhs.timestamp);
//	value += rhs.value;
//	return *this;
//}
//
//Point3d& Point3d::operator-=(Point3d const& rhs)
//{
//	timestamp = max(timestamp, rhs.timestamp);
//	value += rhs.value;
//	return *this;
//}
//
//Point3d& Point3d::operator*=(Point3d const& rhs)
//{
//	timestamp = max(timestamp, rhs.timestamp);
//	value += rhs.value;
//	return *this;
//}
//
//Point3d& Point3d::operator/=(Point3d const& rhs)
//{
//	timestamp = max(timestamp, rhs.timestamp);
//	value += rhs.value;
//	return *this;
//}
//
//
//Point3d Point3d::operator*(Value const& rhs) const
//{
//	return Point3d(value * rhs.value, max(timestamp, rhs.timestamp), sceneMeta);
//}
//
//Point3d Point3d::operator/(Value const& rhs) const
//{
//	return Point3d(value * rhs.value, max(timestamp, rhs.timestamp), sceneMeta);
//}


// Point3d <op> Skeleton3d applies op to each joint in skeleton
Skeleton3d Point3d::operator+(Skeleton3d const& rhs) const
{
	return rhs + *this;
}

Skeleton3d Point3d::operator-(Skeleton3d const& rhs) const
{
	return rhs - *this;
}

Skeleton3d Point3d::operator*(Skeleton3d const& rhs) const
{
	return rhs * *this;
}

Skeleton3d Point3d::operator/(Skeleton3d const& rhs) const
{
	return rhs / *this;
}


//// Point3d <op> Scene3d applies op to each skeleton in scene
//Scene3d Point3d::operator+(Scene3d const& rhs) const
//{
//	return rhs + *this;
//}
//
//Scene3d Point3d::operator-(Scene3d const& rhs) const
//{
//	return rhs - *this;
//}
//
//Scene3d Point3d::operator*(Scene3d const& rhs) const
//{
//	return rhs * *this;
//}
//
//Scene3d Point3d::operator/(Scene3d const& rhs) const
//{
//	return rhs / *this;
//}


bool Point3d::operator==(Point3d const& rhs) const
{
	return value == rhs.value;
}

bool Point3d::operator!=(Point3d const& rhs) const
{
	return value != rhs.value;
}

