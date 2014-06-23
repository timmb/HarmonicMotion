#include "Point2d.h"

using namespace hm;

std::ostream & Point2d::printTo(std::ostream & out) const
{
	return out << value;
}

Point2d::Point2d(ci::Vec2f const& v, double timestamp, int id, SceneMetaPtr sceneMeta)
: Base2dData(timestamp, id, sceneMeta)
, value(v)
{}

Point2d::Point2d(float x, float y, double timestam, int id, SceneMetaPtr sceneMeta)
: Base2dData(timestamp, id, sceneMeta)
, value(x, y)
{}

Point2d::Point2d(double timestamp, int id, SceneMetaPtr sceneMeta)
: Base2dData(timestamp, id, sceneMeta)
{}

Point2d::Point2d(Value const& x, Value const& y)
: Base2dData(chooseTimestamp(x, y), chooseId(x, y), chooseSceneMeta(x, y))
, value(x.value, y.value)
{}

