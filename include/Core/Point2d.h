#pragma once

#include "BaseData.h"
#include "cinder/Vector.h"
#include "Value.h"

namespace cinder
{
	inline Vec2f operator/(float x, Vec2f const& y)
	{
		return Vec2f(x/y.x, x/y.y);
	}
	
	inline Vec2f operator/(double x, Vec2f const& y)
	{
		return Vec2f(x/y.x, x/y.y);
	}
	
	inline Vec2f operator/(hm::Value x, Vec2f const& y)
	{
		return Vec2f(x.value/y.x, x.value/y.y);
	}

}

namespace hm
{
	class Point2d : public Base2dData
	{
	public:
		typedef Base2dData BaseType;
		
		Point2d(ci::Vec2f const& v, double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta = SceneMeta::defaultSceneMeta());
		
		Point2d(float x, float y, double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta = SceneMeta::defaultSceneMeta());
		
		explicit
		Point2d(double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta = SceneMeta::defaultSceneMeta());
		
		Point2d(Value const& x, Value const& y);
		
		ci::Vec2f value;
		
		virtual Type type() const { return POINT2D; }
		virtual std::ostream& printTo(std::ostream& out) const override;
		
		hm_data_define_self_operators_and_assigns(Point2d)
		hm_data_define_scalar_operators_and_assigns(Point2d)
		hm_data_define_equality_ops(Point2d)
	};
	
	hm_data_define_free_mult_scalar_operators(Point2d);
}