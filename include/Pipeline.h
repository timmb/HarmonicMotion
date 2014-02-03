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
		
		/// Starts processing nodes
		void start();
		void stop();
		
		// TODO: Save/load from json
		// Json should be encoded UTF 8
		
	private:
		std::unique_ptr<boost::thread> mThread;
		std::vector<NodePtr> mNodes;
		std::atomic<bool> mIsRunning;
	};
	
}

