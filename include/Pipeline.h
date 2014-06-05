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
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>


namespace hm
{
	typedef boost::upgrade_lock<boost::shared_mutex> UpgradableLock;

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
		
		std::vector<NodePtr> nodes() const;
		std::vector<PatchCordPtr> patchCords() const;
		/// \param path e.g. "My node" or "/My node/
		///
		NodePtr nodeFromPath(std::string path) const;
		/// \param path e.g. "/My node/My outlet"
		OutletPtr outletFromPath(std::string const& path) const;
		InletPtr inletFromPath(std::string const& path) const;
		
		// MARK: Modify pipeline
		
		/// \warning This function must not be called by a node while it is
		/// processing or the thread will deadlock.
		void addNode(NodePtr node);
		/// \warning This function must not be called by a node while it is
		/// processing or the thread will deadlock.
		void removeNode(NodePtr node);
		/// Removes \p oldNode and adds \p newNode. This function will attempt
		/// to recreate any patch cords that were on \p oldNode if possible.
		/// \p newNode will be enabled/disabled based on the state of \p oldNode.
		/// \warning This function must not be called by a node while it is
		/// processing or the thread will deadlock.
		void replaceNode(NodePtr oldNode, NodePtr newNode);
		/// Delete all patchcords and nodes
		/// \warning This function must not be called by a node while it is
		/// processing or the thread will deadlock.
		void clear();
		
		/// Connect an outlet to an inlet.
		/// \return true if the connection was succcessfully made.
		/// \warning This function must not be called by a node while it is
		/// processing or the thread will deadlock.
		bool connect(OutletPtr outlet, InletPtr inlet);
		/// Disconnect an outlet from an inlet
		/// \return true if a connection existed and was removed, or
		/// false if no connection existed.
		/// \warning This function must not be called by a node while it is
		/// processing or the thread will deadlock.
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
		bool isRunning() const { return mIsRunning; }
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
		/// \copydoc fromJson(Json::Value const&, std::vector<std::string>&)
		bool fromJsonString(std::string const& jsonString, std::vector<std::string> & errors);
		/// Write JSON to file
		/// \return true if the file was saved OK.
		bool saveJson(std::string filePath) const;
		/// Read JSON from file, overwriting the current state of this pipeline
		/// with its values.
		bool loadJson(std::string filePath);
		
	private:
		// When we perform actions on the pipeline, listener events are
		// created. Rather than notify listeners immediately, we queue
		// them up and wait until the pipeline mutex is unlocked before
		// sending them. This way if a listener tries to modify the
		// pipline within its callback then we avoid a deadlock.
		
		struct ListenerEvent
		{
			/// Notify \p listener of the event associated with this object
			virtual void notify(Listener* listener) = 0;
		};
		typedef std::shared_ptr<ListenerEvent> ListenerEventPtr;
		typedef std::vector<ListenerEventPtr> ListenerEvents;
		
		struct NodeAddedEvent : public ListenerEvent
		{
			NodePtr node;

			NodeAddedEvent(NodePtr node_) : node(node_) {}
			virtual void notify(Listener* listener);
		};
		
		struct NodeRemovedEvent : public ListenerEvent
		{
			NodePtr node;

			NodeRemovedEvent(NodePtr node_) : node(node_) {}
			virtual void notify(Listener* listener);
		};
		
		struct PatchCordAddedEvent : public ListenerEvent
		{
			PatchCordPtr patchCord;

			PatchCordAddedEvent(PatchCordPtr patchCord_) : patchCord(patchCord_) {}
			virtual void notify(Listener* listener);
		};
		
		struct PatchCordRemovedEvent : public ListenerEvent
		{
			PatchCordPtr patchCord;

			PatchCordRemovedEvent(PatchCordPtr patchCord_) : patchCord(patchCord_) {}
			virtual void notify(Listener* listener);
		};
		
		// MARK: Private functions with strict mutex requirements.
		// *********
		// * Only functions here may modify the pipeline. They do not
		// lock any mutexes - the caller must provide locks as required
		// by the comments. Locks are on mPipelineMutex unless otherwise
		// specified.
		// * These functions do not notify listeners as doing so while
		// a unique/upgradable lock is active could cause a deadlock
		// if the listener callback attempts a further pipeline modification.
		// Instead they return a ListenerEvents vector that the caller
		// needs to process once the pipeline mutex is unlocked.
		// *********
		
		/// \pre Requires a unique lock to be active.
		/// Disconnect and delete a patchcord
		ListenerEvents p_Disconnect(PatchCordPtr p);
		
		/// \pre Requires an unique lock to be active
		ListenerEvents p_AddNode(NodePtr node);
		
		/// \pre Requires a unique lock to be active and node to be
		/// a member of mNodes
		ListenerEvents p_RemoveNode(NodePtr node);
		
		/// \pre Requires a unique lock to be active
		ListenerEvents p_Clear();
		
		/// \pre Requires a unique lock to be active
		ListenerEvents p_Connect(OutletPtr outlet, InletPtr inlet);

		/// \pre Requires a unique lock to be active
		ListenerEvents p_Disconnect(OutletPtr outlet, InletPtr inlet);
		
		/// \pre Requires a shared lock to be active
		bool p_IsConnected(OutletPtr outlet, InletPtr inlet) const;
		
		/// \pre Requires a shared lock to be active
		/// \return The \c PatchCordPtr instance that connectes \p outlet
		/// and \p inlet, or \c nullptr if none exists
		PatchCordPtr p_FindPatchCord(OutletPtr outlet, InletPtr inlet) const;
		
		/// \return Whether the invariant for patch cords holds
		/// \pre Requires a shared lock to be active
		bool p_PatchCordInvariant() const;
		
		/// \note This function grabs a shared lock on mListenersMutex.
		/// \pre Requires no unique or upgrade lock of mPipelineMutex to be
		/// active, and no unique or upgrade lock of mListenersMutex to be
		/// active.
		void p_Process(ListenerEvents const& events) const;
		
		
		std::unique_ptr<boost::thread> mThread;
		std::vector<NodePtr> mNodes;
		std::atomic<bool> mIsRunning;
		/// All patch cords in the pipeline, sorted by inlet
		std::list<PatchCordPtr> mPatchCords;
		std::list<Listener*> mListeners;
		
		/// Guards against simultaneous updating (by this class's threads)
		/// and changing of pipeline
		mutable boost::shared_mutex mPipelineMutex;
		/// Guards against simultaneous updating/notification of mListeners
		mutable boost::shared_mutex mListenersMutex;
		
		friend std::ostream& operator<<(std::ostream&, Pipeline const&);
	};
	
	std::ostream& operator<<(std::ostream&, Pipeline const&);
	
}

