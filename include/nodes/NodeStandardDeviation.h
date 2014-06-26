#pragma once

#include "NodeUnthreaded.h"

namespace hm
{
	class NodeStandardDeviation : public NodeUnthreaded
	{
	public:
NodeStandardDeviation(Node::Params params, std::string className = "NodeStandardDeviation");
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		
	};
}