//
//  Point3d.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "Point3d.h"
#include "cinder/gl/gl.h"

namespace hm
{
	std::ostream& operator<<(std::ostream& out, Point3d const& rhs)
	{
		return out << static_cast<DataType const&>(rhs);
	}
}


using namespace hm;

	std::ostream& Point3d::printTo(std::ostream& out) const
	{
		return out << *static_cast<ci::Vec3f const*>(this);
	}

void Point3d::draw()
{
	ci::gl::drawSphere(*this, 5);
}
