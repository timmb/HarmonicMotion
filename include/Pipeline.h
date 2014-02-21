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
		/// Override this class and register it to be notified of updates
		/// to the Pipeline.
		class Listener
		{
		public:
			virtual void nodeAdded(NodePtr node) = 0;
			/// Nodes are guaranteed not to be removed if they are still
			/// referenced by active patchcords (equivalently if they still
			/// hold reference to any patchcords).
			virtual void nodeRemoved(NodePtr node) = 0;
			virtual void patchCordAdded(OutletPtr outlet, InletPtr inlet) = 0;
			virtual void patchCordRemoved(OutletPtr outlet, InletPtr inlet) = 0;
		};
		
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
		
		/// Register a listener to be notified of updates to this pipeline.
		/// Listeners are notified in the order that they are added, from
		/// the same thread that caused the modification to the pipeline.
		/// \param listener must remain valid for the duration of this
		/// pipeline, or until it is removed.
		void addListener(Listener* listener);
		/// \return true if \p listener was registered and has been
		/// removed, false if \p listener was not registered.
		bool removeListener(Listener* listener);
		
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
		std::list<Listener*> mListeners;
	};
	
}

