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
#include "FactoryNode.h"

using namespace std;
//using boost::unique_lock;
//using boost::chrono::duration;
////using boost::chrono::duration::milli;
//using boost::timed_mutex;
typedef boost::unique_lock<boost::shared_mutex> UniqueLock;
typedef boost::shared_lock<boost::shared_mutex> SharedLock;

namespace hm
{
	
	Inlet::Inlet(Types types,
				 Node& node,
				 string const& name,
				 string const& helpText)
	: Let(types, node, name, helpText)
	, mDataTimestamp(-42)
	, mNumConnections(0)
	, mDestructorHasBeenCalled(false)
	//, mNodeName("(mNodeName unset)")
	{
		assert(types!=UNDEFINED);
	}
	
	void Inlet::detachOwnerNode()
	{
		UniqueLock lock(mMutex);
		Let::detachOwnerNode();
		mWaitCondition.notify_all();
		setNotifyCallback(nullptr);
	}
	
	Inlet::~Inlet()
	{
		mDestructorHasBeenCalled = true;
	}
	
	std::ostream& operator<<(std::ostream& out, Inlet const& inlet)
	{
		return out << "[Inlet "<<inlet.numConnections()<<" connections]";
	}
	
	
	std::string Inlet::toString() const
	{
		return (stringstream() << *this).str();
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

	
	Data Inlet::data() const
	{
		boost::shared_lock<boost::shared_mutex> lock(mMutex);
		return mData;
	}
	
	Data Inlet::dataIfNewerThan(double timestamp) const
	{
		boost::shared_lock<boost::shared_mutex> lock(mMutex);
		return mData.timestamp()>timestamp? mData : Data();
	}
	
	double Inlet::dataTimestamp() const
	{
		boost::shared_lock<boost::shared_mutex> lock(mMutex);
		return mData.timestamp();
	}
	
	OutletPtr Inlet::dataSource() const
	{
		boost::shared_lock<boost::shared_mutex> lock(mMutex);
		return mOutletThatProvidedData;
	}
	
	void Inlet::setNotifyCallback(std::function<void (double)> function)
	{
		UniqueLock lock(mNotifyCallbackMutex);
		mNotifyCallback = function;
	}
	
	
	bool Inlet::waitForNewData(double lastTimestampReceived) const
	{
		boost::unique_lock<boost::shared_mutex> lock(mMutex);
		while (true)
		{
			if (mDestructorHasBeenCalled)
				return false;
			if (node().lock() == nullptr)
				return false;
			if (mDataTimestamp > lastTimestampReceived)
				return true;
			mWaitCondition.wait(lock);
		}
	}
	
	void Inlet::provideNewData(Data const& data, OutletPtr outlet)
	{
		if (!(data.type() & types()))
		{
			return;
		}
		{
			UniqueLock lock(mMutex);
			if (node().lock() != nullptr)
			{
				mData = data;
				mOutletThatProvidedData = outlet;
				mDataTimestamp = data.timestamp();
			}
			//	hm_debug("New data at inlet ("+mNodeName+"): "+data.toString());
			mWaitCondition.notify_all();
		}
		{
			SharedLock lock(mNotifyCallbackMutex);
			if (node().lock() != nullptr && mNotifyCallback != nullptr)
				mNotifyCallback(mDataTimestamp);
		}
	}
	
}



