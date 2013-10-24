//
//  Outlet.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once
#include "Common.h"
#include "Data.h"

namespace hm
{
	class Outlet
	{
	public:
		Outlet(Types type, std::string const& name, std::string const& helpText);
		
		Types types() const { return mTypes; }
		std::string name() const { return mName; }
		std::string helpText() const { return mHelpText; }
		
		/// \return false if cannot connect with inlet due to being wrong
		/// type
		bool connect(InletPtr inlet);
		void outputNewData(Data& data);
		/// \return true if this outlet is connected to any inlets
		int numInlets() const { return mOutputs.size(); }
		
		/// Set the node that this outlet is attached to. Nodes
		/// do this automatically when an outlet is added
		void setNode(Node* node);
//		void setNodeName(std::string const& nodeName) { mNodeName = nodeName; }
		std::string nodeName() const;
		
	private:
		Node* mNode;
		Types mTypes;
		std::string mName;
		std::string mHelpText;
//		std::string mNodeName;
		std::vector<InletPtr> mOutputs;
	};
	
}