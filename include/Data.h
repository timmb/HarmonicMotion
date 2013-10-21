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
		/// Creates a null (i.e. undefined) Data object with timestamp
		/// -42.
		Data();
		Data(Value& value, double timestamp);

		Type type() const { return mType; }
		double timestamp() const { return mTimestamp; }
		
		bool isValue() const;
		Value const& asValue() const;
		Value& asValue();
		
		std::string toString() const;
		
	private:
		typedef boost::variant<Value> Variant;
		Type mType;
		Variant mData;
		double mTimestamp;
		
		friend std::ostream& operator<<(std::ostream&, Data&);
	};
	
	std::ostream& operator<<(std::ostream&, Data&);
}