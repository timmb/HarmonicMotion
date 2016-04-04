#pragma once

#include "NodeUnthreaded.h"
#include <boost/circular_buffer.hpp>

namespace hm
{

	class MovingAverage
	{
	public:
		MovingAverage();
		Data update(Data const& newValue);
		void clear();

		int mNumSamples;

	private:
		/// Sum of data in mBuffer
		Data mCurrentSum;
		boost::circular_buffer<Data> mBuffer;
	};


	class NodeMovingAverage : public NodeUnthreaded
	{
	public:
		NodeMovingAverage(Node::Params params, std::string className = "NodeMovingAverage");
		

	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		double mLastTimestamp;
		MovingAverage mMovingAverage;
	};
}

#include "FactoryNode.h"
hm_register_node(NodeMovingAverage);
