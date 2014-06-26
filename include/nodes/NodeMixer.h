#pragma once

#include "NodeUnthreaded.h"

namespace hm
{
	class NodeMixer : public NodeUnthreaded
	{
	public:
		NodeMixer(Node::Params params, std::string className = "NodeMixer");
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		double mBlendFactor;
		
		double mLastTimestamp0;
		double mLastTimestamp1;
	};
}

#include "FactoryNode.h"
hm_register_node(NodeMixer, "Mixer", "Mixes between two sources of data");
