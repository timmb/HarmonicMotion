//
//  Settings.cpp
//
//  Created by Tim Murray-Browne on 20/02/2013.
//
//

//#ifdef CINDER_CINDER

#include "Parameter.h"
#include <iostream>
#include <ctime>

using namespace std;

namespace hm {
	
	// MARK: BaseParameter
	
	BaseParameter::BaseParameter(std::string const& path)
	: mPath(path)
	{
		// TODO: Verify path is well formed
		// TODO: Check path is unique
	}

	
	void BaseParameter::writeJson(Json::Value& root) const
	{
		Json::Value& child = getChild(root, mPath);
		toJson(child);
	}
	
	
	bool BaseParameter::readJson(Json::Value const& root)
	{
		Json::Value const& child = getChild(root, mPath);
		return fromJson(child);
	}
}


// MARK: JSON Overloads

namespace Json
{
	
	// To avoid having to specialise the Parameter class, we overload
	// the streaming operator for all supported datatypes
	bool operator>>(Json::Value const& child, double& value)
	{
		if (!child.isConvertibleTo(Json::realValue))
			return false;
		value = child.asDouble();
		return true;
	}
	
	bool operator>>(Json::Value const& child, int& value)
	{
		if (!child.isConvertibleTo(Json::intValue))
			return false;
		value = child.asInt();
		return true;
	}
	
	bool operator>>(Json::Value const& child, ci::Vec3f& value)
	{
		if (child["x"].isConvertibleTo(Json::realValue)
			&& child["y"].isConvertibleTo(Json::realValue)
			&& child["z"].isConvertibleTo(Json::realValue))
		{
			value.x = child["x"].asFloat();
			value.y = child["y"].asFloat();
			value.z = child["z"].asFloat();
			return true;
		}
		return false;
	}
	
	bool operator>>(Json::Value const& child, std::string& value)
	{
		if (!child.isConvertibleTo(Json::stringValue))
			return false;
		value = child.asString();
		return true;
	}
	
	Json::Value& operator<<(Json::Value& lhs, ci::Vec3f const& rhs)
	{
		lhs["x"] = rhs.x;
		lhs["y"] = rhs.y;
		lhs["z"] = rhs.z;
		return lhs;
	}

}