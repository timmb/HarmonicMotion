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
	// testing stuff
	class Pipeline
	{
	public:
		std::vector<NodePtr> nodes() const;
		
		void addNode(NodePtr node);
		void removeNode(NodePtr node);
		
	private:
		std::vector<NodePtr> mNodes;
	};
	
}

