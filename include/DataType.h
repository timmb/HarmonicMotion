//
//  DataType.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once
#include <string>
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
		virtual std::string toString() const = 0;
		virtual Type type() const = 0;
	};
}