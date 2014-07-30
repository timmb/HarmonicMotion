//
//  DataNull.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 23/10/2013.
//
//

#include "DataNull.h"
using namespace hm;

DataNull::DataNull(double timestamp, int id, SceneMetaPtr sceneMeta)
: BaseData(timestamp, id, sceneMeta)
{}

std::ostream& DataNull::printTo(std::ostream& out) const
{
	return out << "Null";
}