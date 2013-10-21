//
//  Node.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Node.h"
#include "Inlet.h"

using namespace hm;
using namespace std;

Node::Node(string const& className)
: mClassName(className)
, mThreadIsRequestedToStop(false)
, mStartHasBeenCalled(false)
, mThreadIsRunning(false)
, mThread(nullptr)
{
	assert(mClassName != "");
	hm_debug("Node constructed: "+className);
}

Node::~Node()
{
	{
		boost::unique_lock<boost::mutex> lock(mWasNotifiedMutex);
		mThreadIsRequestedToStop = true;
	}
	mWasNotifiedWaitCondition.notify_all();
	if (mStartHasBeenCalled)
	{
		assert(mThread != nullptr);
		// wait 2 seconds then give up.
		bool ended = mThread->timed_join(boost::posix_time::seconds(2.));
		assert(ended);
	}
}

void Node::start()
{
	mStartHasBeenCalled = true;
	mThreadIsRunning = true;
	mThread = std::unique_ptr<boost::thread>(new boost::thread([this](){ run(); mThreadIsRunning = false; }));
}

InletPtr Node::inlet(int index)
{
	assert(0 <= index && index < mInlets.size());
	return mInlets[index];
}

OutletPtr Node::outlet(int index)
{
	assert(0 <= index && index < mOutlets.size());
	return mOutlets[index];
}

std::string Node::toString() const
{
	stringstream ss;
	ss << "Node "<<mClassName<<" inlets:"<<mInlets.size()<<" outlets:"
	<<mOutlets.size()<<" isRunning:"<<isRunning();
	return ss.str();
}

void Node::addInlet(InletPtr inlet)
{
	if (mStartHasBeenCalled)
	{
		// TODO: error message
		assert(!mStartHasBeenCalled);
		return;
	}
	inlet->setNotifyCallback([=]() { this->callbackNewInletData(); });
	mInlets.push_back(inlet);
}

void Node::addOutlet(OutletPtr outlet)
{
	if (mStartHasBeenCalled)
	{
		// TODO: error message
		assert(!mStartHasBeenCalled);
		return;
	}
	mOutlets.push_back(outlet);
}


void Node::callbackNewInletData()
{
	{
		boost::unique_lock<boost::mutex>(mHasNewInletDataMutex);
		mWasNotified = true;
	}
	mWasNotifiedWaitCondition.notify_all();
}


bool Node::waitForNewData(int inlet) const
{
	if (inlet==-1)
	{
		boost::unique_lock<boost::mutex>(mWasNotifiedMutex);
		mWasNotified = false;
		while (true)
		{
			if (mWasNotified)
				return true;
			if (mThreadIsRequestedToStop)
				return false;
			mWasNotifiedWaitCondition.wait(mWasNotifiedMutex);
		}
	}
	else if (inlet < mInlets.size())
	{
		InletPtr in = mInlets[inlet];
		return in->waitForNewData(in->dataTimestamp());
	}
	assert(inlet < mInlets.size());
	return false;
}