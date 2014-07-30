#pragma once

#include "NodeUnthreaded.h"

namespace hm
{
	class NodeSpeedAcceleration : public NodeUnthreaded
	{
	public:
		NodeSpeedAcceleration(Node::Params params, std::string className = "NodeSpeedAcceleration");
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		double mLastTimestamp;
		Data mLastData;
		Data mLastVelocity;
		Data mLastSpeed;
	};
}
#include "FactoryNode.h"
hm_register_node(NodeSpeedAcceleration)
