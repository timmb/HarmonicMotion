//
//  NodeThreaded.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once

#include "Node.h"
#include <atomic>

namespace boost
{
	class thread;
}

namespace hm
{
	class NodeThreaded : public hm::Node
	{
	public:
		virtual ~NodeThreaded();
		
		/// Starts the thread
		void start();
		/// Thread is running
		bool isRunning() const { return mThreadIsRunning; }

		void disconnectAllCallbacks();
		
	protected:
		/// NodeThreadeds cannot be directly constructed as they are always
		/// subclassed.
		NodeThreaded(Node::Params const& params, std::string const& className);
		
		/// \return true if new data arrived. Otherwise, derived
		/// class should check to see whether it should close
		/// and then recall this function if good to continue.
		/// New data is defined as one or more inlets having
		/// new data.
		/// \param inlet Which inlet to wait for, or -1 for any inlet
		bool waitForNewData(int inlet=-1) const;
		
		/// Override this: Process incoming data until isRequestedToStop()
		/// becomes true
		virtual void run() = 0;
		/// Return from run() at the earliest opportunity when this
		/// becomes false. The NodeThreaded may or may not be started again after
		/// it stops.
		bool isRequestedToStop() const { return mThreadIsRequestedToStop; }
		/// This function must be called in the derived class's destructor
		/// to ensure the thread ends before the derived class's member variables
		/// are destroyed. It will block until thread closes or it times out.
		/// (currently set at 2 seconds).
		void stopAndWait();
		
		virtual InletPtr createInlet(Types types, std::string const& name, std::string const& helpText="") override;

		
	private:
		/// Callback for inlets
		void callbackNewInletData();
		/// Callback for when the thread ends
		void callbackThreadEnded();
		/// Function run by the thread. Delegates to run()
		void threadFunction();
		
		// Set to true when new inlet data arrives
		mutable bool mWasNotified;
		mutable boost::mutex mWasNotifiedMutex;
		mutable boost::condition_variable mWasNotifiedWaitCondition;
		std::atomic<bool> mThreadIsRequestedToStop;
		bool mStartHasBeenCalled;
		bool mStopAndWaitHasBeenCalled;
		
		std::atomic<bool> mThreadIsRunning;
		std::unique_ptr<boost::thread> mThread;
	};
	
	std::ostream& operator<<(std::ostream&, NodeThreaded const&);
}