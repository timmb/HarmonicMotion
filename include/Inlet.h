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
#include "Let.h"
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/condition_variable.hpp>

namespace hm
{
	class Inlet : public Let
	{
	public:
		virtual ~Inlet();
		
		
		int numConnections() const { return mNumConnections; }
		
		Data data() const;
		double dataTimestamp() const;
		/// Returns a pointer to the outlet that provided the current data held
		/// in this inlet. If data().isNull() then dataSource() may be nullptr.
		OutletPtr dataSource() const;
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
//        /// If this inlet is owned by a node and that node was was
//        /// created by FactoryNode then this function will return a weak
//        /// pointer to the node. Otherwise it returns
//        /// std::weak_ptr<Node>()
//        std::weak_ptr<Node> node() const;
		virtual std::string toString() const override;
		
	private:
		/// Type may be a combination of Type flags.
		Inlet(Types types, Node& node, std::string const& name, std::string const& helpText);

		// Accessed by Node ------------
		/// This is used by the node when it is destroyed or this Let
		/// is removed from the node.
		/// There is no need to call it normally.
		/// Once this is called, no more callbacks will be called,
		/// any threads waiting for data from this inlet will be
		/// sent away empty handed and any new data arriving will be ignored.
		/// This will also clear the callback function.
		virtual void detachOwnerNode() override;
		// -------------------------------
		
		// Accessed by Outlet ------------
		void provideNewData(Data const& data, OutletPtr fromOutlet);
		// -------------------------------

		// Accessed by Pipeline ----------
		void incrementNumConnections();
		void decrementNumConnections();
		// -------------------------------
		
//		Types mTypes;
//		std::string mName;
//		std::string mHelpText;
//		std::string mNodeName;
		/// Guarded by mMutex
		Data mData;
		OutletPtr mOutletThatProvidedData;
		/// Guarded by mMutex
		double mDataTimestamp;
		std::atomic<int> mNumConnections;
		
		/// Guard for mData
		mutable boost::shared_mutex mMutex;
		/// Wait condition for mData
		mutable boost::condition_variable_any mWaitCondition;
		/// Guard for notify callback
		mutable boost::shared_mutex mNotifyCallbackMutex;
		std::function<void(double)>mNotifyCallback;
		bool mDestructorHasBeenCalled;
		
		friend class Node;
		friend class Outlet;
		friend class Pipeline;
		friend std::ostream& operator<<(std::ostream&, Inlet const&);
	};
}



