//
//  Node.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once

#include "Data.h"
#include <boost/thread/thread.hpp>
#include <atomic>
#include <set>
#include <string>
#include "Common.h"

namespace hm
{
	class Node
	{
	public:
		/// Universal settings for all nodes. NB Params is not the same
		/// as a Parameter
		struct Params
		{
			/// Name of the node. Will be autogenerated if left blank. All
			/// nodes must have a unique name and name will be adjusted if
			/// necessary to enforce this.
			std::string name;
			
			Params(std::string const& name_="")
			: name(name_) {}
			virtual ~Params() {}
		};
		
		virtual ~Node();
		std::string type() const { return mClassName; }
		std::string name() const { return nodeParams().name; }
				
		int numInlets() const;
		InletPtr inlet(int index);
		const InletPtr inlet(int index) const;

		int numOutlets() const;
		OutletPtr outlet(int index);
		const OutletPtr outlet(int index) const;
		
		std::vector<ParameterPtr> parameters();
		
		
		std::string toString() const;

		/// Enabled is a node-specific, user adjustable setting. When disabled,
		/// a node should ignore all input and does not produce new output.
		/// step() will not be called
		virtual void setEnabled(bool isEnabled);
		bool isEnabled() const { return mIsEnabled; }
		
		// Functions called by the owning pipeline
		/// Requests the node prepares to start processing. Delegates to start().
		void startProcessing();
		/// Requests that the node processes one frame of data from its inlets.
		/// Delegates to step() if isEnabled is true.
		void stepProcessing();
		/// Requests that the node stops processing data. Delegates to stop().
		void stopProcessing();
		/// \return true if we are between startProcessing and stopProcessing calls
		/// (even if isEnabled is false)
		bool isProcessing() const;
		
	protected:
		/// Nodes cannot be directly constructed as they are always
		/// subclassed.
		Node(Params params, std::string const& className);
		
		/// This function may only be used at construction otherwise a runtime error
		/// will be thrown.
		virtual InletPtr createInlet(Types types, std::string const& name, std::string const& helpText="");
		/// This function may only be used at construction otherwise a runtime error
		/// will be thrown.
		virtual OutletPtr createOutlet(Types types, std::string const& name, std::string const& helpText="");
		/// Register a parameter. The base class takes ownership of the parameter.
		/// You need to ensure that the pointer associated with the parameter stays
		/// valid until the end of this node's lifetime.
		/// Parameter effects (callbacks and updates from external sources) only take
		/// effect during a call to updateParameters, which you should do at
		/// the beginning of every step() call (or equivalently frequently if using
		/// NodeThreaded).
		void addParameter(BaseParameter* parameter);
		/// Updates all parameters, activating callbacks where applicable. Callbacks
		/// run in the same thread as this function. You should call this function
		/// at the beginning of your step() function (or at an equivalent rate if
		/// using NodeThreaded).
		/// \note The values associated with the parameters must not be changed
		/// during a call to this function.
		void updateParameters();
		
		// MARK: Abstract functions to implement
		/// This function will be called by the owning pipeline when processing
		/// of data begins. It is guaranteed to be called before the first step()
		/// call and before the first step() call after a stop() call.
		virtual void start() = 0;
		/// This function will be called by the main pipeline continually while
		/// processing is active and this node is enabled. This will be called
		/// from an arbitrary thread but not from separate threads concurrently.
		virtual void step() = 0;
		/// This function is called when processing ends and guarantees that step()
		/// will not be called again until after another call to start()
		virtual void stop() = 0;
		
		

		
		// MARK: Accessors
		Params nodeParams() const;
		/// \param params may be modified by this function to ensure it has
		/// valid values
		void setNodeParams(Params& params);
		
	private:
		std::vector<InletPtr> mInlets;
		std::vector<OutletPtr> mOutlets;
		std::vector<ParameterPtr> mParameters;
		mutable boost::shared_mutex mParametersMutex;
		Params mNodeParams;
		mutable boost::shared_mutex mNodeParamsMutex;
		const std::string mClassName;
		std::atomic<bool> mIsEnabled;
		std::atomic<bool> mIsProcessing;
		std::atomic<bool> mHasStartEverBeenCalled;

		static std::set<std::string> sNamesInUse;
		static boost::mutex sNamesInUseMutex;
	};
	
	std::ostream& operator<<(std::ostream&, Node const&);
}