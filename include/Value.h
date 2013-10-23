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
		
	private:
		double mValue;
	};
}