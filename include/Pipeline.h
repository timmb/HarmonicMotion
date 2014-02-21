//
//  Pipeline.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once
#include "Common.h"
#include <vector>
#include <atomic>
#include <boost/thread.hpp>


namespace hm
{
	class Pipeline
	{
	public:
		Pipeline();
		virtual ~Pipeline();
		
		std::vector<NodePtr> const& nodes() const;
		
		void addNode(NodePtr node);
		void removeNode(NodePtr node);
		
		/// Connect an outlet to an inlet.
		/// \return true if the connection was succcessfully made.
		bool connect(OutletPtr outlet, InletPtr inlet);
		/// Disconnect an outlet from an inlet
		/// \return true if a connection existed and was removed, or
		/// false if no connection existed.
		bool disconnect(OutletPtr outlet, InletPtr inlet);
		/// \return true if \p outlet is connected to \p inlet
		bool isConnected(OutletPtr outlet, InletPtr inlet) const;
		
		/// Starts processing nodes
		void start();
		void stop();
		
		// TODO: Save/load from json
		// Json should be encoded UTF 8
		
	private:
		/// \return Whether the invariant for patch cords holds
		bool patchCordInvariant() const;
		
		std::unique_ptr<boost::thread> mThread;
		std::vector<NodePtr> mNodes;
		std::atomic<bool> mIsRunning;
		/// All patch cords in the pipeline, sorted by inlet
		std::list<PatchCordPtr> mPatchCords;
	};
	
}

