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

	Image2d maximum(Image2d const& lhs, Image2d const& rhs)
	{
		cv::Range rowRange(0, std::max(lhs.value.rows, rhs.value.rows));
		cv::Range colRange(0, std::max(lhs.value.cols, rhs.value.cols));
		cv::Mat lhsCrop = lhs.value(rowRange, colRange);
		cv::Mat rhsCrop = rhs.value(rowRange, colRange);
		cv::Mat rhsConverted;
		if (rhs.type() != lhs.type())
		{
			rhsCrop.convertTo(rhsConverted, lhs.type());
		}
		else
		{
			rhsConverted = rhsCrop;
		}
		Image2d result = Image2d(cv::max(lhsCrop, rhsConverted), chooseTimestamp(lhs, rhs), chooseId(lhs, rhs), chooseSceneMeta(lhs, rhs));
		if (result.sceneMeta->viewportSize != ci::fromOcv(result.value.size()))
		{
			SceneMeta* s = new SceneMeta(*result.sceneMeta);
			s->viewportSize = ci::fromOcv(result.value.size());
			result.sceneMeta = SceneMetaPtr(s);
		}
		return result;
	}



	Value minimum(Value const& lhs, Value const& rhs)
	{
		return lhs.value < rhs.value ? Value(lhs.value, chooseTimestamp(lhs, rhs), chooseId(lhs, rhs), chooseSceneMeta(lhs, rhs)) : Value(rhs.value, chooseTimestamp(lhs, rhs), chooseId(lhs, rhs), chooseSceneMeta(lhs, rhs));
	}

	ListValue minimum(ListValue const& lhs, ListValue const& rhs)
	{
		ListValue out(chooseTimestamp(lhs, rhs), chooseId(lhs, rhs), chooseSceneMeta(lhs, rhs));
		out.value.reserve(std::max(lhs.value.size(), rhs.value.size()));
		auto lhs_it = lhs.value.begin();
		auto rhs_it = rhs.value.begin();
		for (; lhs_it != lhs.value.end() && rhs_it != rhs.value.end(); ++lhs_it, ++rhs_it)
		{
			out.value.push_back(minimum(*lhs_it, *rhs_it));
		}
		// iterate through the remaining values of the longer list
		assert(lhs_it == lhs.value.end() || rhs_it == rhs.value.end());
		for (; lhs_it != lhs.value.end(); ++lhs_it)
		{
			out.value.push_back(*lhs_it);
		}
		for (; rhs_it != rhs.value.end(); ++rhs_it)
		{
			out.value.push_back(*rhs_it);
		}
		return out;
	}

	Image2d minimum(Image2d const& lhs, Image2d const& rhs)
	{
		cv::Range rowRange(0, std::max(lhs.value.rows, rhs.value.rows));
		cv::Range colRange(0, std::max(lhs.value.cols, rhs.value.cols));
		cv::Mat lhsCrop = lhs.value(rowRange, colRange);
		cv::Mat rhsCrop = rhs.value(rowRange, colRange);
		cv::Mat rhsConverted;
		if (rhs.type() != lhs.type())
		{
			rhsCrop.convertTo(rhsConverted, lhs.type());
		}
		else
		{
			rhsConverted = rhsCrop;
		}
		Image2d result = Image2d(cv::min(lhsCrop, rhsConverted), chooseTimestamp(lhs, rhs), chooseId(lhs, rhs), chooseSceneMeta(lhs, rhs));
		if (result.sceneMeta->viewportSize != ci::fromOcv(result.value.size()))
		{
			SceneMeta* s = new SceneMeta(*result.sceneMeta);
			s->viewportSize = ci::fromOcv(result.value.size());
			result.sceneMeta = SceneMetaPtr(s);
		}
		return result;
	}
}




