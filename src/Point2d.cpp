#include "Point2d.h"

using namespace hm;

std::ostream & Point2d::printTo(std::ostream & out) const
{
	return out << value;
}