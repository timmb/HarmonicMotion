//
//  Inlet.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Inlet.h"
#include <boost/chrono.hpp>
#include <boost/thread.hpp>

using namespace hm;
using namespace std;
//using boost::unique_lock;
//using boost::chrono::duration;
////using boost::chrono::duration::milli;
//using boost::timed_mutex;

Inlet::Inlet(Type type,
			 string const& name,
			 string const& helpText)
: mType(type)
, mName(name)
, mHelpText(helpText)
, mDestructorHasBeenCalled(false)
{
	assert(type!=UNDEFINED);
}

Inlet::~Inlet()
{
	mDestructorHasBeenCalled = true;
}

void Inlet::incrementNumConnections()
{
	mNumConnections++;
}

void Inlet::decrementNumConnections()
{
	assert(mNumConnections>0);
	mNumConnections--;
}

void Inlet::setNotifyCallback(std::function<void ()> function)
{
	mNotifyCallback = function;
}


bool Inlet::waitForNewData(double lastTimestampReceived) const
{
	boost::unique_lock<boost::mutex> lock(mMutex);
	while (true)
	{
		if (mDestructorHasBeenCalled)
			return false;
		if (mDataTimestamp > lastTimestampReceived)
			return true;
		mWaitCondition.wait(lock);
	}
}

void Inlet::provideNewData(Data const& data)
{
	boost::unique_lock<boost::mutex> lock(mMutex);
	mData = data;
	mDataTimestamp = data.timestamp();
	mWaitCondition.notify_all();
	if (mNotifyCallback)
		mNotifyCallback();
}



