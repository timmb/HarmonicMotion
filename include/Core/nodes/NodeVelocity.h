#pragma once

#include "NodeUnthreaded.h"

namespace hm
{
	class NodeVelocity : public NodeUnthreaded
	{
	public:
NodeVelocity(Node::Params params, std::string className = "NodeVelocity");
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		
	};
}