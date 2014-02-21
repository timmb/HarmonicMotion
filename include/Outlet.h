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
		
        bool isConnectedTo(InletPtr inlet) const;
		void outputNewData(Data& data);
		/// \return The number of inlets this outlet is connected to.
		int numInlets() const { return mPatchCords.size(); }
		
        /// If this inlet is owned by a node and that node was was
        /// created by FactoryNode then this function will return a weak
        /// pointer to the node. Otherwise it returns
        /// std::weak_ptr<Node>(nullptr)
        std::weak_ptr<Node> node() const;
		
		std::string toString() const { // TODO: implement
			return "Outlet";
		}
		
	private:
		/// Outlets are constructed with a reference to their owning node
		/// The Node will automatically have this outlet registered with
		/// it.
		/// Outlets are only created by Nodes
		Outlet(Types type, Node& owner, std::string const& name, std::string const& helpText);
		/// This is used by the node when it is destroyed just in case the
		/// outlet outlives its node. There is no need to call it normally
		void detachOwnerNode();
		
		
		/// Connections are all managed exclusively by the owning pipeline.
		/// Responsibility lies with the pipeline to ensure any added
		/// patch cords are valid (i.e. they do not exist and there is
		/// at least one shared datatype available between the outlet
		/// and inlet).
		/// Responsibility lies with Pipeline to ensure \p p is valid and
		/// that \p p is removed when this node is removed from the pipeline
		/// (failure to do so will cause a memory leak due to the symmetric
		/// shared_ptr references).
		void addPatchCord(PatchCordPtr patchCord);
		/// Connections are all managed exclusively by the owning pipeline.
		/// Responsibility lies with the pipeline to ensure patchCords are
		/// removed from both outlet and inlet when removed from either.
		void removePatchCord(PatchCordPtr patchCord);
		std::vector<PatchCordPtr> patchCords() const { return mPatchCords; }
//		/// \return false if cannot connect with inlet due to being wrong
//		/// type.
//		bool connect(InletPtr inlet);
//        /// Disconnects an inlet that was previously connected using
//        /// \c connect(InletPtr). If there is no connection, this either
//        /// fails an assertion (in debug) or fails silently (in release).
//        void disconnect(InletPtr inlet);
		
		Node* mNode;
		Types mTypes;
		std::string mName;
		std::string mHelpText;
//		std::string mNodeName;
//		std::vector<std::weak_ptr<Inlet>> mOutputs;
		std::vector<PatchCordPtr> mPatchCords;
		
		friend class Node;
		friend class Pipeline;
	};
	
}


