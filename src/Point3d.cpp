//
//  Point3d.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "Point3d.h"
#include <sstream>
#include "cinder/gl/gl.h"

namespace hm
{
	std::ostream& operator<<(std::ostream& out, Point3d const& rhs)
	{
		return out << static_cast<ci::Vec3f const&>(rhs);
	}
}

using namespace hm;

void Point3d::draw()
{
	ci::gl::drawSphere(*this, 5);
}

std::string Point3d::toString() const
{
	return (std::stringstream() << *this).str();
}

