//
//  DataType.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "DataType.h"
#include <sstream>

namespace hm
{
	std::ostream& operator<<(std::ostream& out, DataType const& rhs)
	{
		return rhs.printTo(out);
	}
}

using namespace hm;

std::string DataType::toString() const
{
	return (std::stringstream() << *this).str();
}