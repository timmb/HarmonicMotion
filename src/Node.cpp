//
//  Node.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Node.h"
#include "Inlet.h"
#include "Outlet.h"

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
//	std::cerr << "Node::~Node() stopping" << std::endl;
	disconnectAllCallbacks();
	stopAndWait();
//	std::cerr << "Node::~Node() stopped" << std::endl;
}

void Node::stopAndWait()
{
	hm_debug("Destructor called on "+toString());
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

void Node::disconnectAllCallbacks()
{
	for (InletPtr inlet: mInlets)
	{
		inlet->setNotifyCallback(nullptr);
	}
}

void Node::start()
{
	mStartHasBeenCalled = true;
	mThread = std::unique_ptr<boost::thread>(new boost::thread([this](){ threadFunction(); }));
}

void Node::threadFunction()
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
	inlet->setNotifyCallback([=](double) { this->callbackNewInletData(); });
	// for now just use type name for node name
	inlet->setNodeName(type());
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
	// for now just use type name for node name
	outlet->setNodeName(type());
	mOutlets.push_back(outlet);
}


void Node::callbackNewInletData()
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


bool Node::waitForNewData(int inlet) const
{
	if (inlet==-1)
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
	else if (inlet < mInlets.size())
	{
		InletPtr in = mInlets[inlet];
		return in->waitForNewData(in->dataTimestamp());
	}
	assert(inlet < mInlets.size());
	return false;
}