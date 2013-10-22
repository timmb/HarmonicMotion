//
//  Point3d.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once
#include "DataType.h"
#include "cinder/Vector.h"

namespace hm
{
	class Point3d : public ci::Vec3f, public DataType
	{
	public:
		Point3d() : ci::Vec3f() {}
		Point3d(ci::Vec3f const& v) : ci::Vec3f(v) {}
		
		virtual void draw();
		virtual std::string toString() const override;
		virtual Type type() const override { return POINT3D; }
		
	};
	std::ostream& operator<<(std::ostream&, Point3d const&);
}