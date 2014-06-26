#pragma once
#include <boost/circular_buffer.hpp>
#include "Common.h"

namespace hm
{
	/// Circular buffer that is time-limited rather than solely value limited.
	template <typename T>
	class TimeBuffer
	{
	public:
		TimeBuffer(double timeLimit);
		void setTimeLimit(double timeLimit);
		double timeLimit() const;
		
		T const& back() const;
		void push_back(T const& x);
		T const& front() const;
		void pop_front();
		
		void clear();
		int size() const;
		bool empty() const { return mBuffer.empty(); }
		
		/// Get rid of out of date elements from buffer given the current time
		void trim(double currentTime);

		/// Hard limit on buffer size, in seconds
		/// This is set to 50
		static double const MAX_TIME_LIMIT;
		
	private:
		boost::circular_buffer_space_optimized<T> mBuffer;
		double mTimeLimit;
	};
	
	template <typename T>
	double const TimeBuffer<T>::MAX_TIME_LIMIT = 100;
	
	template <typename T>
	TimeBuffer<T>::TimeBuffer(double timeLimit)
	: mTimeLimit(-42)
	, mBuffer(0)
	{
		setTimeLimit(timeLimit);
	}
	
	template <typename T>
	void TimeBuffer<T>::setTimeLimit(double timeLimit)
	{
		mTimeLimit = std::min(timeLimit, MAX_TIME_LIMIT);
		int maxBufferSize = mTimeLimit * 50;
		mBuffer.set_capacity(maxBufferSize);
		if (!mBuffer.empty())
		{
			trim(getTimestamp(mBuffer.front()));
		}
	}
	
	template <typename T> inline
	void TimeBuffer<T>::push_back(T const& x)
	{
		mBuffer.push_back(x);
		trim(getTimestamp(x));
	}
	
	template <typename T> inline
	T const& TimeBuffer<T>::back() const
	{
		return mBuffer.back();
	}
	
	template <typename T> inline
	T const& TimeBuffer<T>::front() const
	{
		return mBuffer.front();
	}
	
	template <typename T> inline
	void TimeBuffer<T>::pop_front()
	{
		mBuffer.pop_front();
	}
	
	template <typename T>
	void TimeBuffer<T>::trim(double t)
	{
		double oldestAllowed = t - mTimeLimit;
		int i=0;
		for(auto it=mBuffer.rbegin(); it!=mBuffer.rend(); ++it, ++i)
		{
			if (getTimestamp(*it) >= oldestAllowed)
			{
				break;
			}
		}
		assert(i < mBuffer.size());
		mBuffer.resize(mBuffer.size() - i);
	}
	
	template <typename T> inline
	void TimeBuffer<T>::clear()
	{
		mBuffer.clear();
	}
	
	template <typename T> inline
	int TimeBuffer<T>::size() const
	{
		return mBuffer.size();
	}
	
	template <typename T> inline
	double TimeBuffer<T>::timeLimit() const
	{
		return mTimeLimit;
	}
	
}






