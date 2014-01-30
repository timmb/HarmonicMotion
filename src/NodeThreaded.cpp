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

using namespace hm;
using namespace std;
using namespace boost;

NodeThreaded::NodeThreaded(Node::Params const& params, string const& className)
: Node(params, className)
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
	disconnectAllCallbacks();
//	std::cerr << "NodeThreaded::~NodeThreaded() stopped" << std::endl;
}

void NodeThreaded::stopAndWait()
{
	mStopAndWaitHasBeenCalled = true;
	hm_debug("stopAndWait called on "+toString());
	{
		boost::unique_lock<boost::mutex> lock(mWasNotifiedMutex);
		mThreadIsRequestedToStop = true;
	}
	mWasNotifiedWaitCondition.notify_all();
	if (mStartHasBeenCalled)
	{
		assert(mThread != nullptr);
		// wait 2 seconds then give up.
		hm_debug(toString()+": waiting for thread to finish.");
		bool ended = !mThreadIsRunning || mThread->timed_join(boost::posix_time::seconds(2.));
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
}

void NodeThreaded::disconnectAllCallbacks()
{
	for (int i=0; i<numInlets(); i++)
	{
		inlet(i)->setNotifyCallback(nullptr);
	}
}

void NodeThreaded::start()
{
	mStartHasBeenCalled = true;
	mStopAndWaitHasBeenCalled = false;
	mThread = std::unique_ptr<boost::thread>(new boost::thread([this](){ threadFunction(); }));
}

void NodeThreaded::threadFunction()
{
	mThreadIsRunning = true;
//	try
	{
		run();
	}
//	catch (exception& e)
//	{
//		cerr << "Exception from thread for "<<toString()<<':'<<endl
//		<< e.what() << endl;
//		throw e;
//	}
	mThreadIsRunning = false;
}


std::ostream& operator<<(std::ostream& out, NodeThreaded const& node)
{
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