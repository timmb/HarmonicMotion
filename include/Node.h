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
#include "Parameter.h"
#include "json/json.h"

namespace hm
{
	class Node
	{
	public:
		/// Universal instance settings for all nodes. NB Params is not the same
		/// as a Parameter
		struct Params
		{
			/// Name of the node. Will be autogenerated if left blank. All
			/// nodes must have a unique name and name will be adjusted if
			/// necessary to enforce this.
			std::string name;
			/// Specify initial values for parameters. These will only have
			/// effect if the derived node's constructor registers these
			/// parameters and will override the default value specified
			/// there. Any number of parameters may be specified
			/// here (you don't have to specify them all). If the type of
			/// ParameterValueContainer does not match the parameter type
			/// registered under the name then it will be ignored and an
			/// error printed.
			std::map<std::string, ParameterValueContainer> parameterInitialValues;
			/// Location of this node in the GUI
			int guiLocationX;
			int guiLocationY;
			
			Params(std::string const& name_="",
				   std::map<std::string, ParameterValueContainer> const& parameterInitialValues_=std::map<std::string, ParameterValueContainer>())
			: name(name_)
			, parameterInitialValues(parameterInitialValues_)
			, guiLocationX(0)
			, guiLocationY(0)
			{}
			
			virtual ~Params() {}
		};
		
		virtual ~Node();
		std::string type() const { return mClassName; }
		std::string name() const { return nodeParams().name; }
		/// The full path to this node, without a trailing slash.
		/// The path is a slash separated string of the parents of this node
		/// followed by the name of this node.
		/// \note At present nodes do not have parents, so this is just "/<name>"
		std::string path() const;
		void setName(std::string name);
		std::string toString() const;
		/// Export a Params instance that may be used to reproduce this
		/// object in its current state. This will copy all of the current
		/// parameter values into the initial values of the Params object.
		Params exportParams() const;
		/// \param params may be modified by this function to ensure it has
		/// valid values.
		void setNodeParams(Params& params);

		
		int numInlets() const;
		/// \warning If this node changes its characteristics during runtime
		/// (i.e. adds or removes nodes) then the return of this value should
		/// be checked against \c nullptr event if previously you have tested
		/// that \p index < \c numInlets(). (Due to threading.)
		InletPtr inlet(int index) const;
		std::vector<InletPtr> inlets() const;
		
		int numOutlets() const;
		/// \warning If this node changes its characteristics during runtime
		/// (i.e. adds or removes nodes) then the return of this value should
		/// be checked against \c nullptr event if previously you have tested
		/// that \p index < \c numOutlets(). (Due to threading.)
		OutletPtr outlet(int index) const;
		std::vector<OutletPtr> outlets() const;
		
		std::vector<ParameterPtr> parameters();
		std::vector<ParameterConstPtr> parameters() const;
		
		
		
		/// Enabled is a node-specific, user adjustable setting. When disabled,
		/// a node should ignore all input and does not produce new output.
		/// step() will not be called
		virtual void setEnabled(bool isEnabled);
		bool isEnabled() const { return mIsEnabled; }
		
		// Functions called by the owning pipeline
		/// Requests the node prepares to start processing. Override this to
		/// implement code to initialize processing. Call this original function
		/// first.
		virtual void startProcessing();
		/// Requests that the node processes one frame of data from its inlets.
		/// Override this function to implement code that performs a single update.
		/// This function may be called
		/// sequentially from different threads (but not concurrently).
		/// \return true if this node's characteristics have been changed and this
		/// change has not previously caused this function to return true (i.e.
		/// each change should only trigger a true return value one time). False
		/// otherwise.
		virtual bool stepProcessing();
		/// Requests that the node stops processing data. Override this function to
		/// implement code that concludes processing. Call this original function
		/// first.
		virtual void stopProcessing();
		/// \return true if we are between startProcessing and stopProcessing calls
		/// (even if isEnabled is false)
		bool isProcessing() const { return mIsProcessing; }
		
		/// \return A set of all node names currently in use
		static std::set<std::string> nodeNamesInUse();
		
	protected:
		/// Nodes cannot be directly constructed as they are always
		/// subclassed.
		/// \param className should the the name of the bottom-most derived class
		Node(Params params, std::string className);
		
		/// Factory function to create new instances of a node. Derived types
		/// must implement this for nodes to be able to be created.
		/// \return A new instance of the derived Node type constructed with
		/// the \p params provided.
		virtual NodePtr create(Params params) const = 0;
		
		/// Create an inlet with the given \p name. This will print an error
		/// and return \c nullptr if \p name is already in use.
		virtual InletPtr createInlet(Types types, std::string const& name, std::string const& helpText="");
		/// Create an outlet with the given \p name. This will print an error
		/// and return \c nullptr if \p name is already in use.
		virtual OutletPtr createOutlet(Types types, std::string const& name, std::string const& helpText="");
		/// Removes an inlet from this node.
		bool removeInlet(InletPtr inlet);
		/// Removes an outlet from this node.
		bool removeOutlet(OutletPtr outlet);
		/// Create a parameter. A pointer to the parameter is returned which may
		/// be used to customise it.
		/// Parameter effects (callbacks and updates from external sources) only take
		/// effect during a call to updateParameters, which you should do at
		/// the beginning of every step() call (or equivalently frequently if using
		/// NodeThreaded).
		/// The path of the parameter will be the path of the node followed by \p
		/// name.
		/// \note If an initial value for this parameter has been provided through
		/// Node::Params then *value will be set to that during this function. Any
		/// default values for *value should therefore be set before this function
		/// is called.
		/// \param The name of the parameter. This needs to be unique relative to
		/// this node.
		/// \param A pointer to the value that the parameter will be controlling.
		/// \return A shared pointer to the parameter.
		template <typename T>
		ParameterPtrT<T> addParameter(std::string name, T* value);
		/// Updates all parameters, activating callbacks where applicable. Callbacks
		/// run in the same thread as this function. This function is called
		/// before step() within NodeUnthreaded. Within NodeThreaded you
		/// will need to call it yourself each time within your update loop.
		/// \note The values associated with the parameters must not be changed
		/// during a call to this function.
		void updateParameters();
		
		
		
		
		// MARK: Accessors
		/// Return this node's Params object in its current state.
		Params nodeParams() const;
		
//		/// The pipeline this node is contained within. This may be nullptr
//		/// and should be checked each time it is called.
//		Pipeline* pipeline() const { return mPipeline; }
		
	private:
//		friend Pipeline;
		/// FUNCTION TO BE ACCESSED BY PIPELINE ONLY.
//		void setPipeline(Pipeline* pipeline) { mPipeline = pipeline; }
		/////////////////////////////////////
		
		/// We retain a reference to the pipeline. This may be nullptr
//		std::atomic<Pipeline*> mPipeline;
		std::vector<InletPtr> mInlets;
		std::vector<OutletPtr> mOutlets;
		std::vector<ParameterPtr> mParameters;
		mutable boost::shared_mutex mParametersMutex;
		Params mNodeParams;
		mutable boost::shared_mutex mNodeParamsMutex;
		/// Guards changes to the characteristics of the node (i.e.
		/// how many inlets and outlets it has)
		mutable boost::shared_mutex mCharacteristicsMutex;
		std::atomic<bool> mIsEnabled;
		std::atomic<bool> mIsProcessing;
		std::atomic<bool> mHasStartEverBeenCalled;
		/// If node characteristics change after start has been called
		/// then this is set to false until we notify the pipeline of the
		/// event.
		std::atomic_flag mHaveAllCharacteristicChangesBeenReported;
		const std::string mClassName;
		
		static std::set<std::string> sNamesInUse;
		static boost::mutex sNamesInUseMutex;
		
		friend Json::Value& operator<<(Json::Value&, Node const&);
		friend bool operator>>(Json::Value const&, Node&);
	};
	
	typedef std::function<NodePtr (Node::Params)> NodeCreationFunction;
	
	std::ostream& operator<<(std::ostream&, Node const&);
	
	Json::Value& operator<<(Json::Value&, Node const&);
	bool operator>>(Json::Value const&, Node&);
	
	Json::Value& operator<<(Json::Value&, Node::Params const&);
	bool operator>>(Json::Value const&, Node::Params&);
	
	
	template <typename T>
	ParameterPtrT<T> Node::addParameter(std::string name, T* value)
	{
		// Check if we have a custom initial value
		T* initialValue = nullptr;
		for (std::pair<std::string,ParameterValueContainer> i: nodeParams().parameterInitialValues)
		{
			if (i.first == name)
			{
				initialValue = boost::get<T>(&i.second);
				if (initialValue == nullptr)
				{
					hm_error("Node was constructed with an initial value for "
							 "parameter "+name+" that was of the wrong type.");
				}
			}
		}
		std::shared_ptr<Parameter<T>> parameter;
		if (initialValue)
		{
			parameter = std::shared_ptr<Parameter<T>>(new Parameter<T>(*this, name, value, *initialValue));
		}
		else
		{
			parameter = std::shared_ptr<Parameter<T>>(new Parameter<T>(*this, name, value));
		}
		boost::lock_guard<boost::shared_mutex> lock(mParametersMutex);
		mParameters.push_back(parameter);
		return parameter;
	}
}



