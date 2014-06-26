#pragma once
#include "Types.h"

/// Functions that act on Data Types

namespace hm
{
	Value magnitude(Value const& x);
	
	Value magnitude(Point2d const& x);
	
	Value magnitude(Point3d const& x);
	
	ListValue magnitude(Skeleton3d const& x);
	
	// not yet, waiting for Scene1d type
//	Value magnitude(Scene3d const& x);
	
	Value magnitude(Image2d const& x);
	
	template <typename DataType>
	ListValue magnitude(List<DataType> const& x)
	{
		ListValue out(x.timestamp, x.id, x.sceneMeta);
		out.value.reserve(x.value.size());
		for (auto const& y: x.value)
		{
			out.value.push_back(magnitude(y));
		}
		return out;
	}
	
}