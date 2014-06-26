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
}