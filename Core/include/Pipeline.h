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
#include "Event.h"
#include "EventDispatcher.h"

namespace hm
{
	typedef boost::upgrade_lock<boost::shared_mutex> UpgradableLock;

	class Pipeline
	{
	public:
		
		Pipeline();
		virtual ~Pipeline();
		
		std::string toString() const;
		
		// MARK: Access pipeline elements
		
		std::vector<NodePtr> nodes() const;
		
		std::vector<PatchCordPtr> patchCords() const;
		
		/// \param path e.g. "My node" or "/My node/
		NodePtr nodeFromPath(std::string path) const;
		
		/// \param path e.g. "/My node/My outlet"
		OutletPtr outletFromPath(std::string const& path) const;
		
		InletPtr inletFromPath(std::string const& path) const;
		
		/// \return A list of the names of all nodes in this pipeline
		std::vector<std::string> nodeNames() const;
		
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
		
		/// Checks that the given node has a unique name, changing it to
		/// be unique using Node::setName if necessary.
		void ensureNameIsUnique(NodePtr node);
		
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
		
//		// MARK: Listen to pipeline
//		
//		/// Register a listener to be notified of updates to this pipeline.
//		/// Listeners are notified in the order that they are added, from
//		/// the same thread that caused the modification to the pipeline.
//		/// \param listener must remain valid for the duration of this
//		/// pipeline, or until it is removed.
//		void addListener(Listener* listener);
//		
//		/// \return true if \p listener was registered and has been
//		/// removed, false if \p listener was not registered.
//		bool removeListener(Listener* listener);
		
		// MARK: Start/stop pipeline
		
		/// Starts processing nodes
		void start();
		
		bool isRunning() const { return mIsRunning; }
		
		/// Stop the pipeline. This function will block until all worker threads
		/// have finished.
		/// \warning Do not call stop() from a thread that has locked mPipelineMutex
		/// as the main process may require a unique lock before it can stop.
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
		
		/// Use this to listen to pipeline events
		EventDispatcherPtr dispatcher;
		
	private:
		/// Callbacks for all parameters. This immediate triggers an
		/// event to any registered listeners
		void callbackParameterValueChangedExternally(ParameterPtr p);
		/// \copydoc callbackParameterValueChangedExternally
		void callbackParameterValueChangedInternally(ParameterPtr p);
		
		// When we perform actions on the pipeline, Events are
		// created. Rather than notify listeners immediately, we queue
		// them up and wait until the pipeline mutex is unlocked before
		// sending them. This way if a listener tries to modify the
		// pipline within its callback then we avoid a deadlock.
		
		// MARK: Private functions with strict mutex requirements.
		// *********
		// * Only functions here may modify the pipeline. They do not
		// lock any mutexes - the caller must provide locks as required
		// by the comments. Locks are on mPipelineMutex unless otherwise
		// specified.
		// * These functions do not notify listeners as doing so while
		// a unique/upgradable lock is active could cause a deadlock
		// if the listener callback attempts a further pipeline modification.
		// Instead they return a Events vector that the caller
		// needs to process once the pipeline mutex is unlocked.
		// *********
		
		/// Disconnect and delete a patchcord
		/// \pre Requires a unique lock to be active.
		/// \param skipInvariant Normally in debug mode this function
		/// checks the patchcord invariant is true before and after it acts.
		/// Set this to true if you want to skip this check (e.g. if you know
		/// the invariant shouldn't be true before/after calling this function).
		Events p_Disconnect(PatchCordPtr p, bool skipInvariant=false);
		
		/// \pre Requires an unique lock to be active
		Events p_AddNode(NodePtr node);
		
		/// \pre Requires a unique lock to be active and node to be
		/// a member of mNodes
		Events p_RemoveNode(NodePtr node);
		
		/// Changes \p node's name if necessary to ensure all nodes
		/// in the pipeline have a unique name
		/// \pre Requires a shared lock to be active
		Events p_EnsureNameIsUnique(NodePtr node);
		
		/// \pre Requires a unique lock to be active
		Events p_Clear();
		
		/// \pre Requires a unique lock to be active
		Events p_Connect(OutletPtr outlet, InletPtr inlet);

		/// \pre Requires a unique lock to be active
		Events p_Disconnect(OutletPtr outlet, InletPtr inlet);
		
		/// \pre Requires a unique lock to be active
		/// Checks for any patch cords that are attached to Lets that are
		/// detached from their node and attempts to recreate them if
		/// equivalent nodes exist, or deletes them otherwise.
		Events p_RemoveDeadPatchCords();
		
		/// \pre Requires a shared lock to be active
		bool p_IsConnected(OutletPtr outlet, InletPtr inlet) const;
		
		/// \pre Requires a shared lock to be active
		/// \return The \c PatchCordPtr instance that connectes \p outlet
		/// and \p inlet, or \c nullptr if none exists
		PatchCordPtr p_FindPatchCord(OutletPtr outlet, InletPtr inlet) const;
		
		/// \pre Requires a shared lock to be active
		/// \param path e.g. "My node" or "/My node/
		NodePtr p_NodeFromPath(std::string path) const;
		
		/// \pre Requires a shared lock to be active
		/// \param path e.g. "/My node/My outlet"
		OutletPtr p_OutletFromPath(std::string const& path) const;
		
		/// \pre Requires a shared lock to be active
		InletPtr p_InletFromPath(std::string const& path) const;
		
		/// \return Whether the invariant for patch cords holds
		/// \pre Requires a shared lock to be active
		bool p_PatchCordInvariant() const;
		
//		/// \note This function grabs a shared lock on mListenersMutex.
//		/// \pre Requires no unique or upgrade lock of mPipelineMutex to be
//		/// active, and no unique or upgrade lock of mListenersMutex to be
//		/// active.
//		void p_Process(Events const& events) const;
		
		
		std::unique_ptr<boost::thread> mThread;
		std::vector<NodePtr> mNodes;
		std::atomic<bool> mIsRunning;
		/// All patch cords in the pipeline, sorted by inlet
		std::list<PatchCordPtr> mPatchCords;
		std::list<Listener*> mListeners;
		/// Memory of callbacks registered with parameters, for when they need
		/// to be removed
		std::map<BaseParameter*, int> mParameterInternalCallbackHandles;
		std::map<BaseParameter*, int> mParameterExternalCallbackHandles;
		
		/// Guards against simultaneous updating (by this class's threads)
		/// and changing of pipeline
		mutable boost::shared_mutex mPipelineMutex;
		/// Guards against simultaneous updating/notification of mListeners
		mutable boost::shared_mutex mListenersMutex;
		
		friend std::ostream& operator<<(std::ostream&, Pipeline const&);
	};
	
	std::ostream& operator<<(std::ostream&, Pipeline const&);
	
}

