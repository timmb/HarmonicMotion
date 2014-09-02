//
//  NodeThreaded.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "NodeThreaded.h"
#include "cinder/Utilities.h"
#include "Common.h"
#include <boost/thread.hpp>
#include "Inlet.h"

using namespace std;
using namespace boost;

namespace hm
{
	
	NodeThreaded::NodeThreaded(Node::Params params, string className, string type, string description)
	: Node(params, className, type, description)
	, mThreadIsRequestedToStop(false)
	, mStartHasBeenCalled(false)
	, mStopAndWaitHasBeenCalled(false)
	, mThreadIsRunning(false)
	, mThread(nullptr)
	{
		hm_debug("NodeThreaded constructed: "+className);
	}
	
	NodeThreaded::~NodeThreaded()
	{
		if (!mStopAndWaitHasBeenCalled)
		{
			hm_error("stopAndWait() was not called before destruction. Please "
					 "call stopAnWait() in the destructor of any class inheriting "
					 "NodeThreaded.");
			stopAndWait();
		}
		//	std::cerr << "NodeThreaded::~NodeThreaded() stopping" << std::endl;
		//	std::cerr << "NodeThreaded::~NodeThreaded() stopped" << std::endl;
	}
	
	void NodeThreaded::stopAndWait()
	{
		mStopAndWaitHasBeenCalled = true;
		disconnectAllCallbacks();
		hm_debug("stopAndWait called on "+toString());
		{
			// We use the mWasNotifiedMutex to wake up mWasNotifiedWaitCondition
			boost::unique_lock<boost::mutex> lock(mWasNotifiedMutex);
			mThreadIsRequestedToStop = true;
		}
		mWasNotifiedWaitCondition.notify_all();
		if (mStartHasBeenCalled)
		{
			assert(mThread != nullptr);
			// wait 5 seconds then give up.
			hm_debug(toString()+": waiting for thread to finish.");
			bool ended = !mThreadIsRunning || mThread->timed_join(boost::posix_time::seconds(5));
			if (ended)
			{
				hm_debug(toString()+": thread finished successfully.");
			}
			else
			{
				hm_debug(toString()+": thread did not finish in time.");
			}
			assert(ended);
		}
		boost::lock_guard<boost::shared_mutex> finalLock(mBlockDestruction);
	}
	
	void NodeThreaded::disconnectAllCallbacks()
	{
		for (int i=0; i<numInlets(); i++)
		{
			inlet(i)->setNotifyCallback(nullptr);
		}
	}
	
	void NodeThreaded::startProcessing()
	{
		Node::startProcessing();
		mStartHasBeenCalled = true;
		mThreadIsRequestedToStop = false;
		mStopAndWaitHasBeenCalled = false;
		mThread = std::unique_ptr<boost::thread>(new boost::thread([this](){ threadFunction(); }));
	}
	
	void NodeThreaded::stopProcessing()
	{
		Node::stopProcessing();
		{
			boost::lock_guard<boost::mutex> lock(mWasNotifiedMutex);
			mThreadIsRequestedToStop = true;
		}
		mWasNotifiedWaitCondition.notify_all();
	}
	
	void NodeThreaded::threadFunction()
	{
		mThreadIsRunning = true;
		{
			run();
		}
		mThreadIsRunning = false;
	}
	
	
	std::ostream& operator<<(std::ostream& out, NodeThreaded const& node)
	{
		boost::shared_lock<boost::shared_mutex> lock(node.mBlockDestruction);
		return out << "Threaded " << *(Node*)&node;
	}
	
	
	InletPtr NodeThreaded::createInlet(Types types, std::string const& name, std::string const& helpText)
	{
		
		InletPtr inlet = Node::createInlet(types, name, helpText);
		inlet->setNotifyCallback([=](double) { this->callbackNewInletData(); });
		return inlet;
	}
	
	
	void NodeThreaded::callbackNewInletData()
	{
		{
			boost::unique_lock<boost::mutex>(mHasNewInletDataMutex);
			mWasNotified = true;
		}
		if (!mThreadIsRequestedToStop)
		{
			mWasNotifiedWaitCondition.notify_all();
		}
	}
	
	
	bool NodeThreaded::waitForNewData(int inletNumber) const
	{
		assert(-1 <= inletNumber && inletNumber < numInlets());
		if (inletNumber < 0)
		{
			boost::unique_lock<boost::mutex> lock(mWasNotifiedMutex);
			mWasNotified = false;
			while (true)
			{
				if (mWasNotified)
					return true;
				if (mThreadIsRequestedToStop)
					return false;
				mWasNotifiedWaitCondition.wait(lock);
			}
		}
		else
		{
			InletPtr in = inlet(inletNumber);
			return in->waitForNewData(in->dataTimestamp());
		}
		return false;
	}
	
}