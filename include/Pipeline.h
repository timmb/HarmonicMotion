//
//  Pipeline.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once
#include "Common.h"

namespace hm
{
	class Pipeline
	{
	public:
		Pipeline();
		
		std::vector<NodePtr> const& nodes() const;
		
		void addNode(NodePtr node);
		void removeNode(NodePtr node);
		
		/// Starts all nodes
		void start();
		
	private:
		std::vector<NodePtr> mNodes;
		bool mIsRunning;
	};
	
}

