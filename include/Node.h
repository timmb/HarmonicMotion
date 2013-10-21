//
//  Node.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once

#include "Data.h"
#include <boost/thread.hpp>
#include <atomic>

namespace hm
{
	class Node;
	typedef std::shared_ptr<Node> NodePtr;
	
	class Inlet;
	
	class Node
	{
	public:
		virtual ~Node();
		std::string const& type() const { return mClassName; }
		
		/// Starts the thread
		void start();
		/// Thread is running
		bool isRunning() const { return mThreadIsRunning; }
		
		InletPtr inlet(int index);
		OutletPtr outlet(int index);
		
		std::string toString() const;
		
	protected:
		Node(std::string const& className);
		
		/// Cannot be called after start() has been called
		void addInlet(InletPtr inlet);
		/// Cannot be called after start() has been called
		void addOutlet(OutletPtr outlet);
		
		/// \return true if new data arrived. Otherwise, derived
		/// class should check to see whether it should close
		/// and then recall this function if good to continue.
		/// New data is defined as one more more inlets having
		/// new data.
		/// \param inlet Which inlet to wait for, or -1 for any inlet
		bool waitForNewData(int inlet=-1) const;
		
		/// Override this: Process incoming data until isRequestedToStop()
		/// becomes true
		virtual void run() = 0;
		/// Return from run() at the earliest opportunity when this
		/// becomes false
		bool isRequestedToStop() const { return mThreadIsRequestedToStop; }

	private:
		/// Callback for inlets
		void callbackNewInletData();
		/// Callback for when the thread ends
		void callbackThreadEnded();
		/// Function run by the thread. Delegates to run()
		void threadFunction();
		/// An inlet notified of new data. Used by waitForNewData and the callback above
		std::vector<InletPtr> mInlets;
		std::vector<OutletPtr> mOutlets;
		std::string mClassName;
		mutable bool mWasNotified;
		mutable boost::mutex mWasNotifiedMutex;
		mutable boost::condition_variable mWasNotifiedWaitCondition;
		std::atomic<bool> mThreadIsRequestedToStop;
		bool mStartHasBeenCalled;
		
		std::atomic<bool> mThreadIsRunning;
		std::unique_ptr<boost::thread> mThread;
	};
}