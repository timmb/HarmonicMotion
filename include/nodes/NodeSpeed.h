#pragma once

#include "NodeUnthreaded.h"

namespace hm
{
	class NodeSpeed : public NodeUnthreaded
	{
	public:
NodeSpeed(Node::Params params, std::string className = "NodeSpeed");
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		
	};
}