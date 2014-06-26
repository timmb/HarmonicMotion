//
//  Value.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once
#include "BaseData.h"
#include "Common.h"
#include "cinder/Vector.h"
#include <type_traits>

namespace hm
{
	/// Class representing double values
	class Value : public Base1dData
	{
	public:
		typedef Base1dData BaseType;
		
		Value(double value = 0., double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta = SceneMeta::defaultSceneMeta());
		
		Value(float value, double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta = SceneMeta::defaultSceneMeta());
		
		virtual std::ostream& printTo(std::ostream& out) const override;
		virtual Type type() const override { return VALUE; }
		
		double value;

		// defined below as free operators
//		Value operator+(Value const& rhs) const;
//		Value operator-(Value const& rhs) const;
//		Value operator*(Value const& rhs) const;
//		Value operator/(Value const& rhs) const;
		
		Value& operator+=(Value const& rhs);
		Value& operator-=(Value const& rhs);
		Value& operator*=(Value const& rhs);
		Value& operator/=(Value const& rhs);
		
//		Point3d operator*(Point3d const& rhs) const;
//		Point3d operator/(Point3d const& rhs) const;
		Skeleton3d operator*(Skeleton3d const& rhs) const;
		Skeleton3d operator/(Skeleton3d const& rhs) const;
		Scene3d operator*(Scene3d const& rhs) const;
		Scene3d operator/(Scene3d const& rhs) const;
		
//		Image2d operator+(Image2d const& rhs) const;
//		Image2d operator-(Image2d const& rhs) const;
//		Image2d operator*(Image2d const& rhs) const;
//		Image2d operator/(Image2d const& rhs) const;
		
		
		Value operator*(double rhs) const;
		Value operator/(double rhs) const;
		
		Value operator+() const { return *this; }
		Value operator-() const { return Value(-value, timestamp, id, sceneMeta); }
		
//		operator double&() { return value; }
//		operator double() const { return value; }
//		operator float() const { return value; }
		
//		Value& operator+=(Value const& rhs)
//		{
//			mValue += rhs.value();
//			return *this;
//		}
//		
//		Value& operator*=(Value const& rhs)
//		{
//			mValue *= rhs.value();
//			return *this;
//		}
		
		bool operator==(Value const& rhs) const
		{
			return value==value;
		}

	};
	
//	inline
//	Value Value::operator*(double rhs) const
//	{
//		return Value(value * rhs, timestamp, sceneMeta);
//	}
//	
//	inline
//	Value Value::operator/(double rhs) const
//	{
//		return Value(value / rhs, timestamp, sceneMeta);
//	}
//		
//	inline
//	Value operator*(double lhs, Value const& rhs)
//	{
//		return rhs * lhs;
//	}
//
//	inline
//	Value operator/(double lhs, Value const& rhs)
//	{
//		return Value(lhs / rhs.value, rhs.timestamp, rhs.sceneMeta);
//	}
//	
//	inline
//	Value operator+(double lhs, Value const& rhs)
//	{
//		return rhs + lhs;
//	}
//	
//	inline
//	Value operator-(double lhs, Value const& rhs)
//	{
//		return Value(lhs - rhs.value, rhs.timestamp, rhs.sceneMeta);
//	}
	
	hm_data_define_free_scalar_operators(Value)
	
	// Given that Value's are effectively scalars, we also define
	// assignment operators onto scalars, e.g. float *= Value -> float
#define hm_value_define_free_left_scalar_op_assign(op_assign, Type) \
	inline Type operator op_assign(Type lhs, Value const& rhs) \
	{ \
		return lhs op_assign rhs.value; \
	}
	
	hm_value_define_free_left_scalar_op_assign(+=, float)
	hm_value_define_free_left_scalar_op_assign(-=, float)
	hm_value_define_free_left_scalar_op_assign(*=, float)
	hm_value_define_free_left_scalar_op_assign(/=, float)
	hm_value_define_free_left_scalar_op_assign(+=, double)
	hm_value_define_free_left_scalar_op_assign(-=, double)
	hm_value_define_free_left_scalar_op_assign(*=, double)
	hm_value_define_free_left_scalar_op_assign(/=, double)

}

namespace cinder
{
	inline Vec3f operator*=(Vec3f const& x, hm::Value const& y)
	{
		return x *= y.value;
	}
	
	inline Vec3f operator/=(Vec3f const& x, hm::Value const& y)
	{
		return x *= y.value;
	}
	
}



