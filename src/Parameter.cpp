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
#include <boost/assign/list_of.hpp>

using boost::assign::map_list_of;

namespace std
{
	string to_string(hm::BaseParameter::Type type)
	{
		static_assert(hm::BaseParameter::NUM_TYPES==5, "to_string(hm::BaseParameter::Type type) needs to be updated to accommodate all the different types defined.");

		switch (type)
		{
			case hm::BaseParameter::BOOL:
				return "bool";
			case hm::BaseParameter::FLOAT:
				return "float";
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
		
	BaseParameter::BaseParameter(Node& parent, std::string name, std::string description)
	: mName(name)
	, mDescription(description)
	, mParent(parent)
	, mHardMin(-999999999)
	, mHardMax(999999999)
	, mSoftMin(0)
	, mSoftMax(100)
	, mIsVisible(true)
	, mNextHandle(0)
	, mIsDetached(false)
	, mHasEnumerationLabels(false)
	, mHaveCharacteristicsChanged(false)
	{
		// TODO: Verify path is well formed
		// TODO: Check path is unique
	}
	
	BaseParameter::~BaseParameter()
	{
		assert(isDetached());
	}
	
	void BaseParameter::detach()
	{
		mIsDetached = true;
	}
	
	
	std::string BaseParameter::path() const
	{
		return (isDetached()? string("(detached)") : mParent.path()) + '/' + name();
	}
	
	
	std::string BaseParameter::typeString() const
	{
		return to_string(type());
	}
	
	
	void BaseParameter::setEnumerationLabels(vector<string> const& labels)
	{
		mHasEnumerationLabels = true;
		mEnumerationLabels = labels;
		mHardMin = mSoftMin = 0;
		mHardMax = mSoftMax = labels.size() - 1;
		mHaveCharacteristicsChanged = true;
	}
	
	
	void BaseParameter::setBounds(double hardMin, double hardMax, double softMin, double softMax)
	{
		mHardMin = hardMin;
		mHardMax = hardMax;
		mSoftMin = softMin;
		mSoftMax = softMax;
		mHaveCharacteristicsChanged = true;
	}
	
	
	void BaseParameter::setVisible(bool isVisible)
	{
		mIsVisible = isVisible;
		mHaveCharacteristicsChanged = true;
	}
	
	
	int BaseParameter::addNewExternalValueCallback(std::function<void(void)> callbackFunction)
	{
		return addCallback(callbackFunction, mNewExternalValueCallbacks, mNewExternalValueCallbacksMutex);
	}
	
	
	int BaseParameter::addChangeOfCharacteristicsCallback(std::function<void ()> callbackFunction)
	{
		return addCallback(callbackFunction, mChangeOfCharateristicsCallbacks, mChangeOfCharacteristicsCallbacksMutex);
	}
	
	int BaseParameter::addNewInternalValueCallback(function<void()> callbackFunction)
	{
		return addCallback(callbackFunction, mNewInternalValueCallbacks, mNewExternalValueCallbacksMutex);
	}
	
	

	
	bool BaseParameter::removeCallback(int callbackHandle)
	{
		vector<pair<vector<Callback>*, boost::mutex*>> callbackSets = map_list_of
		(&mNewExternalValueCallbacks, &mNewExternalValueCallbacksMutex)
		(&mChangeOfCharateristicsCallbacks, &mChangeOfCharacteristicsCallbacksMutex)
		(&mNewInternalValueCallbacks, &mNewInternalValueCallbacksMutex);
		
		for (const auto& pair: callbackSets)
		{
			vector<Callback>& callbackList = *pair.first;
			boost::lock_guard<boost::mutex> lock(*pair.second);
			for (auto it=callbackList.begin(); it!=callbackList.end(); ++it)
			{
				if (it->second == callbackHandle)
				{
					callbackList.erase(it);
					return true;
				}
			}
		}
		return false;
	}
	
	void BaseParameter::update()
	{
		if (!isDetached())
		{
			if (checkExternalValue())
			{
				boost::lock_guard<boost::mutex> lock(mNewExternalValueCallbacksMutex);
				for (auto & p: mNewExternalValueCallbacks)
				{
					p.first();
				}
			}
			if (mHaveCharacteristicsChanged)
			{
				mHaveCharacteristicsChanged = false;
				boost::lock_guard<boost::mutex> lock(mChangeOfCharacteristicsCallbacksMutex);
				for (auto & p: mChangeOfCharateristicsCallbacks)
				{
					p.first();
				}
			}
		}
	}
	
	template<>
	BaseParameter::Type Parameter<int>::type() const
	{
		return INT;
	}
	
	template<>
	BaseParameter::Type Parameter<float>::type() const
	{
		return FLOAT;
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
		value = max<int>(hardMin(), min<int>(hardMax(), value));
	}
	
	template<>
	void Parameter<double>::validateExternalValue(double& value) const
	{
		value = max<double>(hardMin(), min<double>(hardMax(), value));
	}
	
	template<>
	void Parameter<float>::validateExternalValue(float& value) const
	{
		value = max<float>(hardMin(), min<float>(hardMax(), value));
	}

}


// MARK: JSON Overloads

namespace Json
{
	
	
	// To avoid having to specialise the Parameter class, we overload
	// the streaming operator for all supported datatypes
	bool operator>>(Json::Value const& child, bool& value)
	{
		if (!child.isConvertibleTo(Json::booleanValue))
		{
			return false;
		}
		value = child.asBool();
		return true;
	}

	
	bool operator>>(Json::Value const& child, double& value)
	{
		if (!child.isConvertibleTo(Json::realValue))
			return false;
		value = child.asDouble();
		return true;
	}
	
	bool operator>>(Json::Value const& child, float& value)
	{
		if (!child.isConvertibleTo(Json::realValue))
			return false;
		value = child.asFloat();
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