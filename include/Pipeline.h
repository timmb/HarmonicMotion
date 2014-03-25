//
//  Pipeline.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once
#include "Common.h"
#include "json/json.h"
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
		
		std::string toString() const;
		
		// MARK: Access pipeline elements
		
		std::vector<NodePtr> const& nodes() const;
		/// \param path e.g. "My node" or "/My node/
		///
		NodePtr nodeFromPath(std::string path);
		/// \param path e.g. "/My node/My outlet"
		OutletPtr outletFromPath(std::string path);
		InletPtr inletFromPath(std::string path);
		
		// MARK: Modify pipeline
		
		void addNode(NodePtr node);
		void removeNode(NodePtr node);
		/// Delete all patchcords and nodes
		void clear();
		
		/// Connect an outlet to an inlet.
		/// \return true if the connection was succcessfully made.
		bool connect(OutletPtr outlet, InletPtr inlet);
		/// Disconnect an outlet from an inlet
		/// \return true if a connection existed and was removed, or
		/// false if no connection existed.
		bool disconnect(OutletPtr outlet, InletPtr inlet);
		/// \return true if \p outlet is connected to \p inlet
		bool isConnected(OutletPtr outlet, InletPtr inlet) const;
		
		// MARK: Listen to pipeline
		
		/// Register a listener to be notified of updates to this pipeline.
		/// Listeners are notified in the order that they are added, from
		/// the same thread that caused the modification to the pipeline.
		/// \param listener must remain valid for the duration of this
		/// pipeline, or until it is removed.
		void addListener(Listener* listener);
		/// \return true if \p listener was registered and has been
		/// removed, false if \p listener was not registered.
		bool removeListener(Listener* listener);
		
		// MARK: Start/stop pipeline
		
		/// Starts processing nodes
		void start();
		void stop();
		
		// MARK: Save/load from json
		// Json should be encoded UTF 8
		/// \p json will be wiped and have this pipeline saved to it
		void toJson(Json::Value& json) const;
		/// This will destroy all nodes and patchcords from this pipeline
		/// and create new ones based on the data in \p json.
		/// \return true if any errors were encountered. Messages for each
		/// error will be added to \p errors.
		bool fromJson(Json::Value const& json, std::vector<std::string>& errors);
		/// Write JSON to file
		/// \return true if the file was saved OK.
		bool saveJson(std::string filePath) const;
		/// Read JSON from file, overwriting the current state of this pipeline
		/// with its values.
		bool loadJson(std::string filePath);
		
	private:
		/// \return Whether the invariant for patch cords holds
		bool patchCordInvariant() const;
		/// Disconnect and delete a patchcord
		void disconnect(PatchCordPtr p);
		
		std::unique_ptr<boost::thread> mThread;
		std::vector<NodePtr> mNodes;
		std::atomic<bool> mIsRunning;
		/// All patch cords in the pipeline, sorted by inlet
		std::list<PatchCordPtr> mPatchCords;
		std::list<Listener*> mListeners;
		
		friend std::ostream& operator<<(std::ostream&, Pipeline const&);
	};
	
	std::ostream& operator<<(std::ostream&, Pipeline const&);
	
}

