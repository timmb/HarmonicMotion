//
// NodeExpression.h
// HarmonicMotion
//
// Created by Tim Murray-Browne on 2014-05-27
//


#pragma once

#include "NodeUnthreaded.h"

namespace hm
{
	template <typename Iterator>
	struct NodeExpressionGrammar;
	
	class NodeExpression : public NodeUnthreaded
	{
	public:
		NodeExpression(Node::Params params, std::string className="NodeExpression");
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		std::unique_ptr<NodeExpressionGrammar<std::string::iterator>> mGrammar;
		std::string expression;
	};
}

#include "FactoryNode.h"
hm_register_node(NodeExpression, "Expression", "Write a mathematical expression to define how input is transformed into output.")