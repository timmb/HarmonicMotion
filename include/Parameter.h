//
//  Parameter.h
//
//  Created by Tim Murray-Browne on 20/02/2013.
//
//

#pragma once

#include "json/json.h"
#include "cinder/Vector.h"
#include <string>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include "Common.h"

namespace hm {

	/// Base class of all parameters
	class BaseParameter
	{
	public:
		virtual ~BaseParameter() {}
		
		/// Given the root of a JSON tree, this will read/write
		/// a value based on \c mPath.
		virtual void writeJson(Json::Value& root) const;
		/// \copydoc writeJson
		virtual bool readJson(Json::Value const& root);
		
		/// e.g. "/My Node/My Parameter"
		std::string path() const { return mPath; }
		/// e.g. "My Parameter"
		std::string name() const { return mName; }
		
		/// Register a callback to be alerted when the internal value (corresponding
		/// to the pointer this parameter was constructed with) is altered
		/// externally (e.g. by a GUI). Callbacks are not called until update()
		/// is called, and run in the same thread as that function.
		void addNewExternalValueCallback(std::function<void(void)> callbackFunction);
		/// If an external value has been received, then this writes it to
		/// this parameter's corresponding value pointer, and then calls
		/// any registered callbacks.
		/// The value of the pointer registered with this Parameter must be valid
		/// and not be
		/// modified by another thread during this update() call.
		/// \note This function should only be called by Node::updateParameters().
		void update();
		
		// These members may or may not be applicable depending on the type
		// of parameter
		/// The minimum value that may be received externally (enforced)
		double hardMin;
		/// The maximum value that may be received extenrally (enforced)
		double hardMax;
		/// The minimum value suggested by the user interface (not enforced)
		double softMin;
		/// The maximum value suggested by the user interface (not enforced)
		double softMax;
		
	protected:
		/// \param path Slash separated with the final element being the
		/// user-visible name of this parameter.
		/// e.g. "/Accumulator1/smoothing value"
		/// or "/value"
		BaseParameter(std::string const& path="");

		virtual void toJson(Json::Value& node) const = 0;
		virtual bool fromJson(Json::Value const& node) = 0;
		/// If an external value is pending, derived class should write the
		/// most recent external value to the pointer registered with this
		/// parameter and return true.
		/// Otherwise, derived class should, if applicable, update its external
		/// value and return false.
		virtual bool checkExternalValue() = 0;

	private:
		/// \return the JSON element corresponding to the given path
		template <typename JsonOrConstJson>
		static JsonOrConstJson& getChild(JsonOrConstJson& root, std::string const& path);
		
		std::vector<std::function<void(void)>> mNewExternalValueCallbacks;
		boost::mutex mNewExternalValueCallbacksMutex;
		const std::string mPath;
		const std::string mName;
	};
	
	template <typename T>
	T& BaseParameter::getChild(T& root, std::string const& path)
	{
		auto slashPos = path.find_first_of('/');
		// Treat as a name if there are no slashes or final character is a slash
		if (slashPos==std::string::npos || slashPos==path.size()-1)
		{
			return root[path];
		}
		else
		{
			assert(slashPos < path.size());
			std::string head = path.substr(0, slashPos);
			std::string tail = path.substr(slashPos+1);
			return getChild(root[head], tail);
		}
	}

	
	
	
	// Main class to represent parameters of type T
	template <typename T>
	class Parameter : public BaseParameter
	{
	public:
		/// Create a parameter to wathc and control \p value.
		/// Callbacks and external updates will only take effect when
		/// BaseParameter::update() is called.
		/// NB value must not be changed during a call to
		/// BaseParameter::update()
		Parameter(std::string const& path, T* value)
		: BaseParameter(path)
		, mValue(value)
		, mHasNewExternalValue(false)
		, mNewInternalValueCallbacksIsNotEmpty(false)
		{
			assert(mValue != nullptr);
		}
		
		/// Externally set this parameter from, e.g., a GUI. The actual value
		/// won't be updated until BaseParameter::update is called.
		/// Thread-safe
		void set(T newValue)
		{
			boost::lock_guard<boost::mutex> lock(mExternalValueMutex);
			mExternalValue = newValue;
			mHasNewExternalValue = true;
		}
		// We don't provide a get function as the only guarantee we have over when
		// mValue changes is when we are within checkExternalValue()
		/// Register a callback that is called when the internal value corresponding
		/// to the pointer changes but no new external value has arrived. The
		/// new value is provided as argument to the callback.
		void addNewInternalValueCallback(std::function<void(T)> callbackFunction)
		{
			boost::lock_guard<boost::mutex> lock(mNewInternalValueCallbacksMutex);
			mNewInternalValueCallbacks.push_back(callbackFunction);
			mNewInternalValueCallbacksIsNotEmpty = true;
		}
		
	protected:
		virtual void toJson(Json::Value& child) const override
		{
			child << *mValue;
		}
		
		virtual bool fromJson(Json::Value const& child) override
		{
			T tempValue;
			if (child >> tempValue)
			{
				child >> *mValue;
				return true;
			}
			else
				return false;
		}
		
		virtual bool checkExternalValue()
		{
			// If we have a new external value, write it to mValue
			if (mHasNewExternalValue)
			{
				boost::lock_guard<boost::mutex> lock(mExternalValueMutex);
				*mValue = mExternalValue;
				return true;
			}
			// Otherwise, if callbacks for internal value changes are registered
			// then we need to check if the internal value has changed
			else if (mNewInternalValueCallbacksIsNotEmpty)
			{
				// Do not allow new external values to be provided while we are
				// in the middle of this
				boost::lock_guard<boost::mutex> lock(mExternalValueMutex);
				if (*mValue != mExternalValue)
				{
					mExternalValue = *mValue;
					// Do not allow new callbacks to be registered while we are
					// iterating through them.
					boost::lock_guard<boost::mutex> lock2(mNewInternalValueCallbacksMutex);
					for (auto callback: mNewInternalValueCallbacks)
					{
						callback(*mValue);
					}
				}
			}
			return false;
		}
		
	private:
		T* mValue;
		T mExternalValue;
		boost::mutex mExternalValueMutex;
		std::atomic<bool> mHasNewExternalValue;
		std::vector<std::function<void(T)>> mNewInternalValueCallbacks;
		boost::mutex mNewInternalValueCallbacksMutex;
		std::atomic<bool> mNewInternalValueCallbacksIsNotEmpty;
	};
	
	
	
} // namespace tmb

namespace Json
{
	// To avoid having to specialise the Parameter class, we overload
	// the streaming operator for all supported datatypes
	bool operator>>(Json::Value const& child, double& value);
	bool operator>>(Json::Value const& child, int& value);
	bool operator>>(Json::Value const& child, ci::Vec3f& value);
	bool operator>>(Json::Value const& child, std::string& value);
	
	Json::Value& operator<<(Json::Value& lhs, ci::Vec3f const& rhs);

	template <typename T>
	Json::Value& operator<<(Json::Value& lhs, T const& rhs)
	{
		lhs = rhs;
		return lhs;
	}
	
}
