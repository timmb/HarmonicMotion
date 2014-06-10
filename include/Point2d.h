#pragma once

#include "BaseData.h"
#include "cinder/Vector.h"

namespace hm
{
	class Point2d : public Base2dData
	{
	public:
		typedef Base2dData BaseType;
		
		Point2d(ci::Vec2f const& v, double timestamp = 0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
		Point2d(float x, float y, double timestamp = 0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
		explicit
		Point2d(double timestamp = 0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
		
		Point2d(Value const& x, Value const& y);
		
		ci::Vec2f value;
		
		hm_data_define_self_operators_and_assigns(Point2d)
		hm_data_define_scalar_operators_and_assigns(Point2d)
	};
	
}