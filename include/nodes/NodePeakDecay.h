#pragma once

#include "NodeUnthreaded.h"

namespace hm
{
	class NodePeakDecay : public NodeUnthreaded
	{
	public:
NodePeakDecay(Node::Params params, std::string className = "NodePeakDecay");
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		
	};
}