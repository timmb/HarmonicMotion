//
//  BaseData.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "BaseData.h"
#include <sstream>

namespace hm
{
	std::ostream& operator<<(std::ostream& out, BaseData const& rhs)
	{
		return rhs.printTo(out);
	}
}

using namespace hm;

BaseData::BaseData(double timestamp_, int id_, SceneMetaPtr sceneMeta_)
: id(id_)
, timestamp(timestamp_)
, sceneMeta(sceneMeta_)
{
	assert(sceneMeta != nullptr);
}

std::string BaseData::toString() const
{
	std::stringstream ss;
	ss << *this;
	return ss.str();
}