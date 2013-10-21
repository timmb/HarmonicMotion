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
, mDestructorHasBeenCalled(false)
{
	assert(mClassName != "");
}

Node::~Node()
{
	boost::unique_lock<boost::mutex> lock(mWasNotifiedMutex);
	mDestructorHasBeenCalled = true;
}

void Node::addInlet(InletPtr inlet)
{
	inlet->setNotifyCallback([=]() { this->callbackNewInletData(); });
	mInlets.push_back(inlet);
}

void Node::addOutlet(OutletPtr outlet)
{
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
		while (!mDestructorHasBeenCalled && !mWasNotified)
		{
			mWasNotifiedWaitCondition.wait(mWasNotifiedMutex);
			if (mDestructorHasBeenCalled)
				return false;
			if (mWasNotified)
				return true;
		}
	}
	else if (inlet < mInlets.size())
	{
		InletPtr in = mInlets[inlet];
		return in->waitForNewData(in->dataTimestamp());
	}
	else
	{
		assert(inlet < mInlets.size());
		return false;
	}
}