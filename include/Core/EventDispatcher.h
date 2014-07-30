#pragma once
#include "Event.h"
#include <queue>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition_variable.hpp>
#include <memory>

namespace hm
{
	class Listener;
	/// Class that runs in a thread and dispatches events. This allows
	/// us to trigger events from anywhere regardless if any mutexes are
	/// locked without worrying about the resulting callbacks causing deadlocks
	/// by attempting to modify the pipeline.
	class EventDispatcher
	{
	public:
		EventDispatcher();
		virtual ~EventDispatcher();
		
		void addListener(Listener* listener);
		bool removeListener(Listener* listener);		
		
		void dispatch(EventPtr const& event);
		void dispatch(Events const& events);
		
	private:
		void threadFunction();
		
		bool mIsRunning;
		boost::thread mThread;
		std::queue<EventPtr> mQueue;
		mutable boost::mutex mMutex;
		boost::condition_variable mCondition;
		std::vector<Listener*> mListeners;
	};
	typedef std::shared_ptr<EventDispatcher> EventDispatcherPtr;
}