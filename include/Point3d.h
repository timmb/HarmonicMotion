//
//  Point3d.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once
#include "BaseData.h"
#include "cinder/Vector.h"

namespace hm
{
	class Point3d : public Base3dData
	{
	public:
		typedef Base3dData BaseType;
		
		Point3d(ci::Vec3f const& v, double timestamp = 0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
		Point3d(float x, float y, float z, double timestamp=0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
		explicit
		Point3d(double timestamp = 0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
		/// Constructs a Point3d taking the maximum timestamp from
		/// the given Values. SceneMeta is taken in priority of x,
		/// y and then z, choosing the first non-default SceneMeta
		/// found (or keeping the default if all parameters have the
		/// default SceneMeta)
		Point3d(Value const& x, Value const& y, Value const& z);
		
		ci::Vec3f value;
		
		
		Point3d operator+(Point3d const& rhs) const;
		Point3d operator-(Point3d const& rhs) const;
		/// Pointwise multiplication
		Point3d operator*(Point3d const& rhs) const;
		/// Pointwise division
		Point3d operator/(Point3d const& rhs) const;
		Point3d& operator+=(Point3d const& rhs);
		Point3d& operator-=(Point3d const& rhs);
		/// Pointwise multiplication
		Point3d& operator*=(Point3d const& rhs);
		/// Pointwise division
		Point3d& operator/=(Point3d const& rhs);
		
		Point3d operator*(Value const& rhs) const;
		Point3d operator/(Value const& rhs) const;
		
		
		// Point3d <op> Skeleton3d applies op to each joint in skeleton
		Skeleton3d operator+(Skeleton3d const& rhs) const;
		Skeleton3d operator-(Skeleton3d const& rhs) const;
		Skeleton3d operator*(Skeleton3d const& rhs) const;
		Skeleton3d operator/(Skeleton3d const& rhs) const;
		
		// Point3d <op> Scene3d applies op to each skeleton in scene
		Scene3d operator+(Scene3d const& rhs) const;
		Scene3d operator-(Scene3d const& rhs) const;
		Scene3d operator*(Scene3d const& rhs) const;
		Scene3d operator/(Scene3d const& rhs) const;
		
		template <typename Scalar>
		Point3d operator*(Scalar const& rhs) const;
		template <typename Scalar>
		Point3d operator/(Scalar const& rhs) const;
		template <typename Scalar>
		Point3d& operator*=(Scalar const& rhs);
		template <typename Scalar>
		Point3d& operator/=(Scalar const& rhs);
		
		bool operator==(Point3d const& rhs) const;
		bool operator!=(Point3d const& rhs) const;
		
		Point3d operator+() const { return *this; }
		Point3d operator-() const { return Point3d(-value, timestamp, sceneMeta); }

		virtual void draw();
		virtual std::ostream& printTo(std::ostream& out) const override;
		virtual Type type() const override { return POINT3D; }

	};
	
	template <typename Scalar>
	Point3d Point3d::operator*(Scalar const& rhs) const
	{
		Point3d out;
		out.value *= rhs;
		return out;
	}

	template <typename Scalar>
	Point3d Point3d::operator/(Scalar const& rhs) const
	{
		Point3d out;
		out.value /= rhs;
		return out;
	}
	
	template <typename Scalar>
	Point3d operator*(Scalar const& lhs, Point3d const& rhs)
	{
		return rhs * lhs;
	}

	template <typename Scalar>
	Point3d operator/(Scalar const& lhs, Point3d const& rhs)
	{
		return Point3d(ci::Vec3f(lhs/rhs.value.x, lhs/rhs.value.y, lhs/rhs.value.z), rhs.timestamp, rhs.sceneMeta);
	}
	
	template <typename Scalar>
	Point3d& Point3d::operator*=(Scalar const& rhs)
	{
		value *= rhs;
		return *this;
	}
	
	template <typename Scalar>
	Point3d& Point3d::operator/=(Scalar const& rhs)
	{
		value /= rhs;
		return *this;
	}
	
}





