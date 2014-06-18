#include "Point2d.h"

using namespace hm;

std::ostream & Point2d::printTo(std::ostream & out) const
{
	return out << value;
}

Point2d::Point2d(ci::Vec2f const& v, double timestamp, SceneMetaPtr sceneMeta)
: Base2dData(timestamp, sceneMeta)
, value(v)
{}

Point2d::Point2d(float x, float y, double timestam, SceneMetaPtr sceneMeta)
: Base2dData(timestamp, sceneMeta)
, value(x, y)
{}

Point2d::Point2d(double timestamp, SceneMetaPtr sceneMeta)
: Base2dData(timestamp, sceneMeta)
{}

Point2d::Point2d(Value const& x, Value const& y)
: Base2dData(std::max(x.timestamp, y.timestamp), choose(x.sceneMeta, y.sceneMeta))
, value(x.value, y.value)
{}

