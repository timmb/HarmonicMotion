#pragma once

#include "NodeUnthreaded.h"
#include "utils/TimeBuffer.h"

namespace hm
{
	class NodeMovingAverage : public NodeUnthreaded
	{
	public:
		NodeMovingAverage(Node::Params params, std::string className = "NodeMovingAverage");
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		double mLastTimestamp;
		Data mCurrentAverage;
		double mDuration;
		TimeBuffer<Data> mBuffer;
	};
}

#include "FactoryNode.h"
hm_register_node(NodeMovingAverage, "Moving average", "Finds the average value over the past n seconds");
