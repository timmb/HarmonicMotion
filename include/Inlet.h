//
//  Inlet.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once
#include "Common.h"
#include "Data.h"
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/condition_variable.hpp>

namespace hm
{
	class Inlet
	{
	public:
		/// \param sharedWaitCondition is notified whenever new data
		/// arrives at this inlet
		Inlet(Type type, std::string const& name, std::string const& helpText);
		virtual ~Inlet();
		
		Type type() const { return mType; }
		virtual std::string name() const { return mName; }
		virtual std::string helpText() const { return mHelpText; }
		
		int numConnections() const { return mNumConnections; }
		
		Data data() const;
		double dataTimestamp() const;
		/// will block until:
		/// - This inlet's destructor is called (returning false)
		/// - dataTimestamp() > lastTimestampReceived (returning true)
		bool waitForNewData(double lastTimestampReceived) const;
		/// Set a function to be called when new data arrives.
		/// \note This function must be thread safe and should not block
		void setNotifyCallback(std::function<void(void)> function);
		
	private:
		// Accessed by Outlet ------------
		void provideNewData(Data const& data);
		void incrementNumConnections();
		void decrementNumConnections();
		// --------------------------
		
		Type mType;
		std::string mName;
		std::string mHelpText;
		/// Guarded by mMutex
		Data mData;
		/// Guarded by mMutex
		double mDataTimestamp;
		int mNumConnections;
		
		mutable boost::shared_mutex mMutex;
		mutable boost::condition_variable_any mWaitCondition;
		std::function<void(void)> mNotifyCallback;
		bool mDestructorHasBeenCalled;
		
		friend class Outlet;
	};
}