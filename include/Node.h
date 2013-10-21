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
		
	protected:
		Node(std::string const& className);
		
		void addInlet(InletPtr inlet);
		void addOutlet(OutletPtr outlet);
		
		/// \return true if new data arrived. Otherwise, derived
		/// class should check to see whether it should close
		/// and then recall this function if good to continue.
		/// New data is defined as one more more inlets having
		/// new data.
		/// \param inlet Which inlet to wait for, or -1 for any inlet
		bool waitForNewData(int inlet=-1) const;

	private:
		std::vector<InletPtr> mInlets;
		std::vector<OutletPtr> mOutlets;
		std::string mClassName;
		/// Callback for inlets
		void callbackNewInletData();
		/// An inlet notified of new data. Used by waitForNewData and the callback above
		mutable bool mWasNotified;
		mutable boost::mutex mWasNotifiedMutex;
		mutable boost::condition_variable mWasNotifiedWaitCondition;
		bool mDestructorHasBeenCalled;
	};
}