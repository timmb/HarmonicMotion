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
	
#define HM_REGISTER_NODE(ClassName) namespace { \
	static bool hm__registrar_##ClassName = ([]() { \
		if (!hm::FactoryNode::instance()->hasNodeType(#ClassName)) { \
			hm::FactoryNode::instance()->registerNodeType(hm::NodeInfo(#ClassName, [](hm::Node::Params params) { \
					return hm::NodePtr(new hm::ClassName(params)); \
				})); \
		} return true; \
	})(); \
}
	
	
	/// Information describing a node type required by FactoryNode.
	struct NodeInfo
	{
		/// The name of the class of the node type.
		std::string className;
		/// A function that generates new instances
		/// of the node.
		std::function<NodePtr (Node::Params)> creationFunction;
		
		NodeInfo(std::string className_, std::function<NodePtr (Node::Params)> creationFunction_)
		: className(className_)
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
		
		/// Create a new node of the specified type.
		/// \return A shared pointer to the new node instance, or nullptr if
		/// \p className has not been registered
		NodePtr create(std::string className, Node::Params params = Node::Params()) const;
		
	private:
		std::map<std::string, NodeInfo> mNodeInfos;
	};
	
}