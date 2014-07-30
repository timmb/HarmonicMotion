#pragma once

#include "NodeUnthreaded.h"
#include <boost/circular_buffer.hpp>

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
		/// Sum of data in mBuffer
		Data mCurrentSum;
		int mNumSamples;
		boost::circular_buffer<Data> mBuffer;
	};
}

#include "FactoryNode.h"
hm_register_node(NodeMovingAverage);
