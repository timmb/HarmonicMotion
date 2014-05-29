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
#include "Node.h"

namespace std
{
	string to_string(hm::BaseParameter::Type type)
	{
		static_assert(hm::BaseParameter::NUM_TYPES==3, "to_string(hm::BaseParameter::Type type) needs to be updated to accommodate all the different types defined.");

		switch (type)
		{
			case hm::BaseParameter::DOUBLE:
				return "double";
			case hm::BaseParameter::INT:
				return "int";
			case hm::BaseParameter::STRING:
				return "string";
			default:
				assert(false);
				return "";
		}
	}
}

using namespace std;

namespace hm {
	
	// MARK: BaseParameter
		
	BaseParameter::BaseParameter(Node& parent, std::string name)
	: mName(name)
	, mParent(parent)
	, hardMin(- numeric_limits<double>::max())
	, hardMax(  numeric_limits<double>::max())
	, softMin(0)
	, softMax(100)
//	, mType(type)
	{
		// TODO: Verify path is well formed
		// TODO: Check path is unique
	}
	
	
	std::string BaseParameter::path() const
	{
		return mParent.path() + '/' + name();
	}
	
	std::string BaseParameter::typeString() const
	{
		return to_string(type());
	}

	
//	void BaseParameter::writeJson(Json::Value& root) const
//	{
//		Json::Value& child = getChild(root, path());
//		toJson(child);
//	}
//	
//	
//	bool BaseParameter::readJson(Json::Value const& root)
//	{
//		Json::Value const& child = getChild(root, path());
//		return fromJson(child);
//	}
	
	void BaseParameter::addNewExternalValueCallback(std::function<void(void)> callbackFunction)
	{
		boost::lock_guard<boost::mutex> lock(mNewExternalValueCallbacksMutex);
		mNewExternalValueCallbacks.push_back(callbackFunction);
	}
	
	void BaseParameter::update()
	{
		if (checkExternalValue())
		{
			boost::lock_guard<boost::mutex> lock(mNewExternalValueCallbacksMutex);
			for (auto callback: mNewExternalValueCallbacks)
			{
				callback();
			}
		}
	}
	
	template<>
	BaseParameter::Type Parameter<int>::type() const
	{
		return INT;
	}
	
	template<>
	BaseParameter::Type Parameter<double>::type() const
	{
		return DOUBLE;
	}
	
	template<>
	BaseParameter::Type Parameter<std::string>::type() const
	{
		return STRING;
	}

	template<>
	void Parameter<int>::validateExternalValue(int& value) const
	{
		value = max<int>(hardMin, min<int>(hardMax, value));
	}
	
	template<>
	void Parameter<double>::validateExternalValue(double& value) const
	{
		value = max<double>(hardMin, min<double>(hardMax, value));
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