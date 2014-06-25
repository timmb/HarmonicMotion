#include "EventDispatcher.h"
#include <boost/thread/lock_guard.hpp>

using namespace hm;
using namespace std;

typedef boost::unique_lock<boost::mutex> Lock;

EventDispatcher::EventDispatcher()
: mIsRunning(true)
{
	mThread = boost::thread(std::bind(&EventDispatcher::threadFunction, this));
}


EventDispatcher::~EventDispatcher()
{
	mIsRunning = false;
	mCondition.notify_one();
	mThread.join();
}

void EventDispatcher::addListener(Listener *listener)
{
	assert(listener != nullptr);
	if (listener != nullptr)
	{
		Lock lock(mMutex);
		mListeners.push_back(listener);
	}
}

bool EventDispatcher::removeListener(Listener *listener)
{
	auto it = find(begin(mListeners), end(mListeners), listener);
	if (it != end(mListeners))
	{
		mListeners.erase(it);
		return true;
	}
	return false;
}


void EventDispatcher::dispatch(EventPtr const& event)
{
	assert(event != nullptr);
	Lock lock(mMutex);
	mQueue.push(event);
	mCondition.notify_one();
}

void EventDispatcher::dispatch(Events const& events)
{
	Lock lock(mMutex);
	for (EventPtr const& e: events)
	{
		assert(e!=nullptr);
		mQueue.push(e);
	}
	mCondition.notify_one();
}

void EventDispatcher::threadFunction()
{
	while (mIsRunning)
	{
		EventPtr event;
		{
			Lock lock(mMutex);
			if (mQueue.empty())
			{
				mCondition.wait(lock);
			}
			if (!mIsRunning)
			{
				return;
			}
			if (!mQueue.empty())
			{
				event = mQueue.front();
				assert(event != nullptr);
				mQueue.pop();
			}
		}
		
		// unlock the lock before making any callbacks in case the
		// callback tries to do something that triggers another event
		// which deadlock if mMutex was locked.
		if (event != nullptr)
		{
			for (Listener* listener: mListeners)
			{
				assert(listener != nullptr);
				assert(event != nullptr);
				event->notify(listener);
			}
		}
	}
}




