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
	
	
	
	/// max(T, T) is supported for Scalar types only. Other types
	/// will return DataNull.
	Value maximum(Value const& lhs, Value const& rhs);
	/// max on lists of different sizes will return a list of the greater
	/// size with later values being taken unconditionally from the later
	/// list
	ListValue maximum(ListValue const& lhs, ListValue const& rhs);
	/// max on images will return an image with each pixel the maximum
	/// between the two corresponding pixels in the source images.
	/// The output will be the minimum size of the inputs and the type
	/// of lhs.
	Image2d maximum(Image2d const& lhs, Image2d const& rhs);
	/// Generic function for unordered datatypes
	template <typename T>
	typename std::enable_if<!supports_maximum<T>::value, DataNull>::type
	maximum(T const& lhs, T const& rhs)
	{
		return DataNull();
	}

	Value minimum(Value const& lhs, Value const& rhs);
	ListValue minimum(ListValue const& lhs, ListValue const& rhs);
	Image2d minimum(Image2d const& lhs, Image2d const& rhs);
	template <typename T>
	typename std::enable_if<!supports_minimum<T>::value, DataNull>::type
	minimum(T const& lhs, T const& rhs)
	{
		return DataNull();
	}
}