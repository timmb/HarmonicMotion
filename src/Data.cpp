//
//  Data.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 26/09/2013.
//
//

#include "Data.h"

using namespace hm;
using namespace std;

Data::Data()
: mType(UNDEFINED)
, mData(0.)
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