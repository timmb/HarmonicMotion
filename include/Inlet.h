//
//  Inlet.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once
#include "Common.h"
#include "Data.h"
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/condition_variable.hpp>

namespace hm
{
	class Inlet
	{
	public:
		virtual ~Inlet();
		
		Types types() const { return mTypes; }
		virtual std::string name() const { return mName; }
		virtual std::string helpText() const { return mHelpText; }
		
		int numConnections() const { return mNumConnections; }
		
		Data data() const;
		double dataTimestamp() const;
		/// Returns data if it is newer than \p timestamp, or a null data object
		/// if it is not. Use this to avoid race conditions between dataTimestamp() and data() calls
		Data dataIfNewerThan(double timestamp) const;
		/// will block until:
		/// - This inlet's destructor is called (returning false)
		/// - dataTimestamp() > lastTimestampReceived (returning true)
		bool waitForNewData(double lastTimestampReceived) const;
		/// Set a function to be called when new data arrives.
		/// \param function A callback function called when new data arrives.
		/// The timestamp of the new data is provided as argument to the function
		/// \note This function must be thread safe and should not block
		void setNotifyCallback(std::function<void(double)> function);
//		/// Set the name of the node that this inlet is attached to. Nodes
//		/// do this automatically when an inlet is added
//		void setNodeName(std::string const& nodeName) { mNodeName = nodeName; }
//		std::string nodeName() const { return mNodeName; }
		
	private:
		/// Type may be a combination of Type flags.
		Inlet(Types types, Node& node, std::string const& name, std::string const& helpText);
		/// This is used by the node when it is destroyed just in case the
		/// outlet outlives its node. There is no need to call it normally
		void detachOwnerNode();

		// Accessed by Outlet ------------
		void provideNewData(Data const& data);
		void incrementNumConnections();
		void decrementNumConnections();
		// --------------------------
		
		Node* mNode;
		Types mTypes;
		std::string mName;
		std::string mHelpText;
		std::string mNodeName;
		/// Guarded by mMutex
		Data mData;
		/// Guarded by mMutex
		double mDataTimestamp;
		int mNumConnections;
		
		mutable boost::shared_mutex mMutex;
		mutable boost::condition_variable_any mWaitCondition;
		std::function<void(double)> mNotifyCallback;
		bool mDestructorHasBeenCalled;
		
		friend class Node;
		friend class Outlet;
	};
}