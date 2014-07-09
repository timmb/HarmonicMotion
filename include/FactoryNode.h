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
/// #include "FactoryNode.h"
/// hm_register_node(NodeOscOut)
#define hm_register_node(ClassName) namespace { \
	static bool hm__registrar_##ClassName = ([]() { \
		if (!hm::FactoryNode::instance()->hasNodeClass(#ClassName)) { \
				/* Create a temporary instance, grab the params. */ \
			hm::NodePtr instance(new hm::ClassName(hm::Node::Params())); \
			hm::FactoryNode::instance()->registerNodeType(hm::NodeInfo( \
				#ClassName, \
				instance->type(), \
				instance->description(), \
				instance->exportParams(), \
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
		std::string type;
		/// A brief description of what the node does, as presented to the user
		/// e.g. Sends received data to another application using Open Sound Control (OSC).
		std::string description;
		/// The default Params, which will contain descriptions of all the
		/// parameters registered by the node
		Node::Params params;
		/// A function that generates new instances
		/// of the node.
		NodeCreationFunction creationFunction;
		
		NodeInfo(std::string className_,
				 std::string type_,
				 std::string description_,
				 Node::Params const& params_,
				 NodeCreationFunction creationFunction_)
		: className(className_)
		, type(type_)
		, description(description_)
		, params(params_)
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
		
		/// \return true if \p nodeTypeName has been registered with the factory so
		/// calls to create(\p nodeTypeName) are valid
		bool hasNodeType(std::string nodeTypeName) const;
		
		/// \return true if a node with class \p nodeClassName has been regsitered
		/// \note (Nodes are created using their \c type not their class name
		bool hasNodeClass(std::string nodeClassName) const;
		
		/// This creates a Params object that contains all of the
		/// ParameterInitialValue elements that would be exported by
		/// \p className. Use this, set the parameters and then feed it back
		/// into create() to create a Node.
		/// If \p className is not found then a default constructed Node::Params
		/// is returned
		Node::Params createParams(std::string const& nodeTypeName);
		
		/// Create a new node of the specified type.
		/// \param nodeTypeName The name of the node as presented to the user
		/// \return A shared pointer to the new node instance, or nullptr if
		/// \p className has not been registered
		NodePtr create(std::string nodeTypeName, Node::Params params = Node::Params());
		
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
//		std::map<std::string, Node::Params> mNodeParams;
	};
	
}




