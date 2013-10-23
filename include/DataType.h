//
//  DataType.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once
#include <string>
#include <ostream>
#include "Type.h"

namespace hm
{
	/// Base class for different Types
	class DataType
	{
	public:
		virtual ~DataType() {}
		/// Assumes we are already in a GL context
		virtual void draw() {}
		virtual std::string toString() const;
		virtual Type type() const = 0;
		
		/// Print to the stream and return it
		virtual std::ostream& printTo(std::ostream&) const = 0;
	};
	std::ostream& operator<<(std::ostream&, DataType const&);
}