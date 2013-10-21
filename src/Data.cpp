//
//  Data.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 26/09/2013.
//
//

#include "Data.h"
#include <sstream>

using namespace hm;
using namespace std;


Data::Data()
: mType(UNDEFINED)
, mData(0.)
, mTimestamp(-42.)
{}

Data::Data(Value& value, double timestamp)
: mType(VALUE)
, mData(value)
, mTimestamp(timestamp)
{}

bool Data::isValue() const
{
	return mType == VALUE;
}

Value const& Data::asValue() const
{
	return boost::get<Value const&>(mData);
}

Value& Data::asValue()
{
	return boost::get<Value&>(mData);
}

std::string Data::toString() const
{
	std::stringstream ss;
	switch (mType)
	{
		case VALUE:
			ss << asValue();
			break;
		default:
			ss << "Unable to convert datatype into string";
			break;
	}
	return ss.str();
}

std::ostream& operator<<(std::ostream& out, Data& rhs)
{
	return out << rhs.toString();
}