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
		Types types() const { return mTypes; }
		std::string name() const { return mName; }
		std::string helpText() const { return mHelpText; }
		
		/// \return false if cannot connect with inlet due to being wrong
		/// type
		bool connect(InletPtr inlet);
        bool isConnectedTo(InletPtr inlet) const;
        /// Disconnects an inlet that was previously connected using
        /// \c connect(InletPtr). If there is no connection, this either
        /// fails an assertion (in debug) or fails silently (in release).
        void disconnect(InletPtr inlet);
		void outputNewData(Data& data);
		/// \return true if this outlet is connected to any inlets
		int numInlets() const { return mOutputs.size(); }
		
        /// If this inlet is owned by a node and that node was was
        /// created by FactoryNode then this function will return a weak
        /// pointer to the node. Otherwise it returns
        /// std::weak_ptr<Node>(nullptr)
        std::weak_ptr<Node> node() const;
		
	private:
		/// Outlets are constructed with a reference to their owning node
		/// The Node will automatically have this outlet registered with
		/// it.
		/// Outlets are only created by Nodes
		Outlet(Types type, Node& owner, std::string const& name, std::string const& helpText);
		/// This is used by the node when it is destroyed just in case the
		/// outlet outlives its node. There is no need to call it normally
		void detachOwnerNode();
		
		Node* mNode;
		Types mTypes;
		std::string mName;
		std::string mHelpText;
//		std::string mNodeName;
		std::vector<std::weak_ptr<Inlet>> mOutputs;
		
		friend class Node;
	};
	
}