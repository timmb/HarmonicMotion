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
		double mLastTimestamp;
		double mDecayRate;
		Data mPeak;
	};
}

#include "FactoryNode.h"
hm_register_node(NodePeakDecay, "Peak decay", "Calculates the peak value with an adjustable decay.")
