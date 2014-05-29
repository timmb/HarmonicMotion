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
	namespace expression
	{
		template <typename Iterator>
		struct Grammar;
		struct Program;
	}
	
	class NodeExpression : public NodeUnthreaded
	{
	public:
		NodeExpression(Node::Params params, std::string className="NodeExpression");
		
		bool setExpression(std::string const& expression);
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		void setLetCounts(int numScalarInlets, int numVectorInlets, int numOutlets);
		void expressionChangedCallback();
		
		std::unique_ptr<expression::Grammar<std::string::const_iterator>> mGrammar;
		std::string mExpression;
		std::unique_ptr<expression::Program> mProgram;
		bool mIsValid;
		double mLastTimestamp;
	};
}

#include "FactoryNode.h"
hm_register_node(NodeExpression, "Expression", "Write a mathematical expression to define how input is transformed into output.")