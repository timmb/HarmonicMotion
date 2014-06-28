#include "Functions.h"
#include <math.h>
#include "CinderOpenCV.h"

namespace hm
{
	using std::abs;
	
	Value magnitude(Value const& x)
	{
		return Value(abs(x.value), x.timestamp, x.id, x.sceneMeta);
	}
	
	Value magnitude(Point2d const& x)
	{
		return Value(x.value.length(), x.timestamp, x.id, x.sceneMeta);
	}
	
	Value magnitude(Point3d const& x)
	{
		return Value(x.value.length(), x.timestamp, x.id, x.sceneMeta);
	}
	
	ListValue magnitude(Skeleton3d const& x)
	{
		ListValue out(x.timestamp, x.id, x.sceneMeta);
		out.value.reserve(NUM_JOINTS);
		for (int i=0; i<NUM_JOINTS; i++)
		{
			out.value.push_back(magnitude(x.joint(i)));
		}
		return out;
	}
	
	
	Value magnitude(Image2d const& x)
	{
		return Value(cv::norm(cv::mean(x.value)), x.timestamp, x.id, x.sceneMeta);
	}
	
	
	Value maximum(Value const& lhs, Value const& rhs)
	{
		return lhs.value < rhs.value? Value(rhs.value, chooseTimestamp(lhs, rhs), chooseId(lhs,rhs), chooseSceneMeta(lhs,rhs)) : Value(lhs.value, chooseTimestamp(lhs,rhs), chooseId(lhs,rhs), chooseSceneMeta(lhs,rhs));
	}
	
	ListValue maximum(ListValue const& lhs, ListValue const& rhs)
	{
		ListValue out(chooseTimestamp(lhs, rhs), chooseId(lhs, rhs), chooseSceneMeta(lhs, rhs));
		out.value.reserve(std::max(lhs.value.size(), rhs.value.size()));
		auto lhs_it = lhs.value.begin();
		auto rhs_it = rhs.value.begin();
		for( ; lhs_it!=lhs.value.end() && rhs_it!=rhs.value.end(); ++lhs_it, ++rhs_it)
		{
			out.value.push_back(maximum(*lhs_it, *rhs_it));
		}
		// iterate through the remaining values of the longer list
		assert(lhs_it==lhs.value.end() || rhs_it==rhs.value.end());
		for ( ; lhs_it!=lhs.value.end(); ++lhs_it)
		{
			out.value.push_back(*lhs_it);
		}
		for ( ; rhs_it!=rhs.value.end(); ++rhs_it)
		{
			out.value.push_back(*rhs_it);
		}
		return out;
	}
}




