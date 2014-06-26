#pragma once

#include "NodeUnthreaded.h"

namespace hm
{
	class NodeSpeedAcceleration : public NodeUnthreaded
	{
	public:
NodeSpeedAcceleration(Node::Params params, std::string className = "NodeSpeed");
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		double mLastTimestamp;
		Data mLastData;
		Data mLastSpeed;
	};
}
#include "FactoryNode.h"
hm_register_node(NodeSpeedAcceleration, "Speed and Acceleration", "Calculate the speed and acceleration of values.")
