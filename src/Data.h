//
//  Data.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 26/09/2013.
//
//

#pragma once
#include "Common.h"
#include <boost/variant.hpp>

namespace hm
{
	/// Variant container for all data types
	class Data
	{
	public:
		Data();
		Type type() const { return mType; }
		
		bool isValue() const;
		Value const& asValue() const;
		Value& asValue();
		
	private:
		typedef boost::variant<Value> Variant;
		Type mType;
		Variant mData;
	};
}