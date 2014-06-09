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
#include <boost/variant.hpp>

namespace hm {
	
	/// On occasion it's useful to have a container that can contain any
	/// value that may be represented by a parameter. At the moment this is
	/// used in Node but not in the Parameter classes which use templates
	/// and operator overloading.
	/// Types here should match those in BaseParameter::Type
	typedef boost::variant<double, int, std::string> ParameterValueContainer;

	/// Base class of all parameters
	class BaseParameter
	{
	public:
		/// These types should match those in ParameterValueContainer.
		enum Type
		{
			DOUBLE,
			INT,
			STRING,
			NUM_TYPES
		};
		
		virtual ~BaseParameter();
		
		/// Set \p value to the value of this parameter
		virtual void toJson(Json::Value& value) const = 0;
		/// Set this node to the value of \p value
		virtual bool fromJson(Json::Value const& value) = 0;

		/// e.g. "/My Node/My Parameter"
		std::string path() const;
		/// e.g. "My Parameter"
		std::string name() const { return mName; }
		virtual Type type() const = 0;
		/// \return Type in string format
		std::string typeString() const;
		virtual std::string toString() const = 0;
		/// \return The current value represented by this parameter.
		/// \note If a new external value has arrived and update() has
		/// not yet been called then this will return the new external
		/// value which may not yet be the same as the value actually pointed
		/// to by this parameter.
		virtual ParameterValueContainer toContainer() const = 0;
		
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
		
		void setBounds(double hardMin, double hardMax, double softMin, double softMax);
		// These members may or may not be applicable depending on the type
		// of parameter
		/// The minimum value that may be received externally (enforced where applicable)
		double hardMin() const { return mHardMin; }
		/// The maximum value that may be received extenrally (enforced where applicable)
		double hardMax() const { return mHardMax; }
		/// The minimum value suggested by the user interface (not enforced)
		double softMin() const { return mSoftMin; }
		/// The maximum value suggested by the user interface (not enforced)
		double softMax() const { return mSoftMax; }
		
		/// Only applicable for integer parameters: provide a set of labels
		/// describing each possible value. This will update soft/hard min/
		/// max to the range [0..labels.size())
		void setEnumerationLabels(std::vector<std::string> const& labels);
		bool hasEnumerationLabels() const { return mHasEnumerationLabels; }
		std::vector<std::string> enumerationLabels() const { return mEnumerationLabels; }
		
		// TODO: Add function for Node to detach itself when it is destroyed
		/// Node calls this on its destruction. This will prevent the
		/// parameter from attempting to read/write to the value pointer
		/// or activate any callbacks. It's pretty much useless and
		/// (safely) waiting to be destroyed.
		virtual void detach();
		bool isDetached() const { return mIsDetached; }
		
	protected:
		/// \param path Slash separated with the final element being the
		/// user-visible name of this parameter.
		/// e.g. "/Accumulator1/smoothing value"
		/// or "/value"
		BaseParameter(Node& parent, std::string name);

		/// If an external value is pending, derived class should write the
		/// most recent external value to the pointer registered with this
		/// parameter and return true.
		/// Otherwise, derived class should, if applicable, update its external
		/// value and return false.
		virtual bool checkExternalValue() = 0;

	private:
		double mHardMin;
		double mHardMax;
		double mSoftMin;
		double mSoftMax;
		
		std::vector<std::function<void(void)>> mNewExternalValueCallbacks;
		boost::mutex mNewExternalValueCallbacksMutex;
		Node& mParent;
		const std::string mName;
		bool mIsDetached;
//		const Type mType;
		bool mHasEnumerationLabels;
		std::vector<std::string> mEnumerationLabels;
	};
	
}

namespace std
{
	std::string to_string(hm::BaseParameter::Type type);
}

namespace hm
{
	
	
	
	// Main class to represent parameters of type T
	template <typename T>
	class Parameter : public BaseParameter
	{
	public:
		/// Create a parameter to watch and control \p value.
		/// Callbacks and external updates will only take effect when
		/// BaseParameter::update() is called.
		/// NB value must not be changed during a call to
		/// BaseParameter::update()
		/// \param parent The node to which this parameter belongs
		/// \param name The name of the parameter
		/// \param value A pointer to the value to be controlled by the parameter
		Parameter(Node& parent, std::string name, T* value)
		: BaseParameter(parent, name)
		, mValue(value)
		, mExternalValue(*value)
		, mHasNewExternalValue(false)
		, mNewInternalValueCallbacksIsNotEmpty(false)
		{
			assert(mValue != nullptr);
		}
		
		/// \copydoc Parameter(Node&, std::string, T*)
		/// \param initialValue This is written to *value without triggering
		/// any callbacks
		Parameter(Node& parent, std::string name, T* value, T const& initialValue)
		: BaseParameter(parent, name)
		, mValue(value)
		, mExternalValue(initialValue)
		, mHasNewExternalValue(false)
		, mNewInternalValueCallbacksIsNotEmpty(false)
		{
			assert(mValue != nullptr);
			*mValue = initialValue;
		}
		
		virtual Type type() const override;
		
		virtual std::string toString() const override
		{
			boost::lock_guard<boost::mutex> lock(mExternalValueMutex);
			std::stringstream ss;
			ss << name() << " ("<<typeString()<<") = "<<mExternalValue;
			return ss.str();
		}
		
		/// Externally set this parameter from, e.g., a GUI. The actual value
		/// won't be updated until BaseParameter::update is called.
		/// Thread-safe
		void set(T newValue)
		{
//			std::cout << "new external value: " << newValue << std::endl;
			boost::lock_guard<boost::mutex> lock(mExternalValueMutex);
			mExternalValue = newValue;
//			std::cout << "new mExternalValue: " << mExternalValue << std::endl;
			mHasNewExternalValue = true;
		}
		/// Register a callback that is called when the internal value corresponding
		/// to the pointer changes but no new external value has arrived. The
		/// new value is provided as argument to the callback.
		/// \return A handle that can be used to remove the function
		int addNewInternalValueCallback(std::function<void(T)> callbackFunction)
		{
			boost::lock_guard<boost::mutex> lock(mNewInternalValueCallbacksMutex);
			int handle = mNextNewInternalValueCallbackHandle++;
			mNewInternalValueCallbacks.push_back(std::pair<std::function<void(T)>,int>(callbackFunction, handle));
			mNewInternalValueCallbacksIsNotEmpty = true;
			return handle;
		}
		
		bool removeNewInternalValueCallback(int functionHandle)
		{
			boost::lock_guard<boost::mutex> lock(mNewInternalValueCallbacksMutex);
			for (auto it=mNewInternalValueCallbacks.begin(); it != mNewInternalValueCallbacks.end(); )
			{
				if (it->second == functionHandle)
				{
					mNewInternalValueCallbacks.erase(it);
					if (mNewInternalValueCallbacks.empty())
					{
						mNewInternalValueCallbacksIsNotEmpty = false;
					}
					return true;
				}
				else
				{
					++it;
				}
			}
			return false;
		}
		
		virtual void toJson(Json::Value& value) const override
		{
			// We rely on mExternalValue as our most recently cached value
			// of mValue. This avoids potential thread-safety issues from
			// accessing mValue outside of checkExternalValue().
			boost::lock_guard<boost::mutex> lock(mExternalValueMutex);
			value << mExternalValue;
		}
		
		virtual bool fromJson(Json::Value const& value) override
		{
			T t;
			if (value >> t)
			{
				set(t);
				return true;
			}
			else
			{
				return false;
			}
		}
		
		virtual ParameterValueContainer toContainer() const override
		{
			boost::lock_guard<boost::mutex> lock(mExternalValueMutex);
			ParameterValueContainer v(mExternalValue);
			return v;
		}
		
		virtual void detach()
		{
			BaseParameter::detach();
			mValue = nullptr;
		}
		
	protected:
		
		virtual bool checkExternalValue()
		{
			assert(!isDetached());
			// If we have a new external value, write it to mValue
			if (mHasNewExternalValue)
			{
				boost::lock_guard<boost::mutex> lock(mExternalValueMutex);
				validateExternalValue(mExternalValue);
				*mValue = mExternalValue;
				mHasNewExternalValue = false;
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
						callback.first(*mValue);
					}
				}
			}
			return false;
		}
		
	private:
		/// If applicable, check \p value is in range and valid. (Done in
		/// specialisations of this function.)
		void validateExternalValue(T& value) const {}
		
		T* mValue;
		T mExternalValue;
		mutable boost::mutex mExternalValueMutex;
		std::atomic<bool> mHasNewExternalValue;
		// functions stored with their removal handle
		std::vector<std::pair<std::function<void(T)>, int>> mNewInternalValueCallbacks;
		int mNextNewInternalValueCallbackHandle;
		mutable boost::mutex mNewInternalValueCallbacksMutex;
		std::atomic<bool> mNewInternalValueCallbacksIsNotEmpty;
		
		friend std::ostream& operator<<(std::ostream& out, Parameter<T> const& rhs);
	};
	
	template<>
	BaseParameter::Type Parameter<int>::type() const;
	
	template<>
	BaseParameter::Type Parameter<double>::type() const;
	
	template<>
	BaseParameter::Type Parameter<std::string>::type() const;
	
	template<>
	void Parameter<int>::validateExternalValue(int& value) const;
	
	template<>
	void Parameter<double>::validateExternalValue(double& value) const;
	
	template <typename T>
	std::ostream& operator<<(std::ostream& out, Parameter<T> const& rhs)
	{
		return out << rhs.toString();
	}
	
	
	
	
}

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
