//
//  Value.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once
#include "BaseData.h"

namespace hm
{
	/// Class representing double values
	class Value : public BaseData
	{
	public:
		Value(double value = 0., double timestamp = 0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
		Value(float value, double timestamp = 0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
		
		virtual std::ostream& printTo(std::ostream& out) const override;
		virtual Type type() const override { return VALUE; }
		
		double value;
		
		Value operator+(Value const& rhs) const;
		Value operator-(Value const& rhs) const;
		Value operator*(Value const& rhs) const;
		Value operator/(Value const& rhs) const;
		
		Value& operator+=(Value const& rhs);
		Value& operator-=(Value const& rhs);
		Value& operator*=(Value const& rhs);
		Value& operator/=(Value const& rhs);
		
		Point3d operator*(Point3d const& rhs) const;
		Point3d operator/(Point3d const& rhs) const;
		Skeleton3d operator*(Skeleton3d const& rhs) const;
		Skeleton3d operator/(Skeleton3d const& rhs) const;
		Scene3d operator*(Scene3d const& rhs) const;
		Scene3d operator/(Scene3d const& rhs) const;
		
		
		Value operator*(double rhs) const;
		Value operator/(double rhs) const;
		
		
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
	
	inline
	Value Value::operator*(double rhs) const
	{
		return Value(value * rhs, timestamp, sceneMeta);
	}
	
	inline
	Value Value::operator/(double rhs) const
	{
		return Value(value / rhs, timestamp, sceneMeta);
	}
		
	inline
	Value operator*(double lhs, Value const& rhs)
	{
		return rhs * lhs;
	}

	inline
	Value operator/(double lhs, Value const& rhs)
	{
		return Value(lhs / rhs.value, rhs.timestamp, rhs.sceneMeta);
	}
	
	inline
	Value operator+(double lhs, Value const& rhs)
	{
		return rhs + lhs;
	}
	
	inline
	Value operator-(double lhs, Value const& rhs)
	{
		return Value(lhs - rhs.value, rhs.timestamp, rhs.sceneMeta);
	}
	

}