//
//  FactoryNode.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 05/02/2014.
//  Copyright (c) 2014 Tim Murray-Browne. All rights reserved.
//
#pragma once
#include "Common.h"
#include "Node.h"

namespace hm
{

/// Use this Macro in the header file for defining a node, right after the class
/// declaration. Example usage:
/// class NodeOscOut : public NodeUnthreaded {
///     //...
/// };
/// hm_register_node(NodeOscOut, "OSC out", "Sends received data to another application via OSC")
#define hm_register_node(ClassName, FriendlyName, FriendlyDescription) namespace { \
	static bool hm__registrar_##ClassName = ([]() { \
		if (!hm::FactoryNode::instance()->hasNodeType(#ClassName)) { \
			hm::FactoryNode::instance()->registerNodeType(hm::NodeInfo(\
				#ClassName, \
				FriendlyName, \
				FriendlyDescription, \
				[](hm::Node::Params params) { \
					return hm::NodePtr(new hm::ClassName(params)); \
				})); \
		} return true; \
	})(); \
}

	
	/// Information describing a node type required by FactoryNode.
	struct NodeInfo
	{
		/// The name of the class of the node type.
		/// e.g. NodeOscOut
		std::string className;
		/// The node type name as presented to the user
		/// e.g. OSC out
		std::string friendlyName;
		/// A brief description of what the node does, as presented to the user
		/// e.g. Sends received data to another application using Open Sound Control (OSC).
		std::string friendlyDescription;
		/// A function that generates new instances
		/// of the node.
		NodeCreationFunction creationFunction;
		
		NodeInfo(std::string className_,
				 std::string friendlyName_,
				 std::string friendlyDescription_,
				 NodeCreationFunction creationFunction_)
		: className(className_)
		, friendlyName(friendlyName_)
		, friendlyDescription(friendlyDescription_)
		, creationFunction(creationFunction_)
		{}
	};

	
	/// Singleton factory class to create Node instances.
	class FactoryNode
	{
	public:
		typedef std::shared_ptr<FactoryNode> Ptr;

		static Ptr instance();
		
		/// Register a node type for this factory to be able to create
		/// Throws a std::runtime_error if a node type with that className
		/// has already been registered.
		void registerNodeType(NodeInfo const& nodeInfo);
		
		bool hasNodeType(std::string className) const;
		
		/// This creates a Params object that contains all of the
		/// ParameterInitialValue elements that would be exported by
		/// \p className. Use this, set the parameters and then feed it back
		/// into create() to create a Node.
		/// If \p className is not found then a default constructed Node::Params
		/// is returned
		Node::Params createParams(std::string const& className);
		
		/// Create a new node of the specified type.
		/// \return A shared pointer to the new node instance, or nullptr if
		/// \p className has not been registered
		NodePtr create(std::string className, Node::Params params = Node::Params());
		
		std::vector<NodeInfo> nodeTypes();
        
        /// \return The NodePtr (shared pointer) to an \p existingNode that
        /// was previously constructed by the factory. If \p existingNode was
        /// not constructed by the factory or existingNode has been
        /// destroyed then returns nullptr.
        NodePtr getNodePtr(Node* existingNode) const;
		
	private:
		// Singleton class
		FactoryNode();
		FactoryNode(FactoryNode const&) = delete;
		FactoryNode& operator=(FactoryNode const&) = delete;
        
        /// Removes any references to destroyed nodes
        /// from mCreatedNodes. This is to avoid a minimal memory leak
        /// in the case where many nodes are created and destroyed.
        void cleanUp();
		
		std::map<std::string, NodeInfo> mNodeInfos;
		/// We ensure that  no new nodes are registered after
		/// the create or nodeType functions have been called.
		std::atomic<bool> mNodeFunctionsCalled;
        std::map<Node*, std::weak_ptr<Node>> mCreatedNodes;
		std::map<std::string, Node::Params> mNodeParams;
	};
	
}




