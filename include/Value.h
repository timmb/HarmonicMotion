//
//  Value.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once
#include "DataType.h"

namespace hm
{
	/// Class representing double values
	class Value : public DataType
	{
	public:
		Value(double value = 0.);
		Value(float value);
		
		virtual std::ostream& printTo(std::ostream& out) const override;
		virtual Type type() const override { return VALUE; }
		
		double& value() { return mValue; }
		double const& value() const { return mValue; }
		
		Value operator+(Value const& rhs) const
		{
			return Value(value() + rhs.value());
		}
		
		Value operator-(Value const& rhs) const
		{
			return Value(mValue - rhs.mValue);
		}
		
		Value operator*(Value const& rhs) const
		{
			return Value(value() * rhs.value());
		}
		
		Value& operator+=(Value const& rhs)
		{
			mValue += rhs.value();
			return *this;
		}
		
		Value& operator*=(Value const& rhs)
		{
			mValue *= rhs.value();
			return *this;
		}
		bool operator==(Value const& rhs) const
		{
			return mValue==rhs.mValue;
		}
		
	private:
		double mValue;
	};
	
	template <typename T>
	Value operator*(T const& lhs, Value const& rhs)
	{
		return rhs * lhs;
	}
}