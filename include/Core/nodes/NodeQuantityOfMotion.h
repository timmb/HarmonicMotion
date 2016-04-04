#pragma once

#include "NodeUnthreaded.h"
#include "Image2d.h"
#include <boost/circular_buffer.hpp>

namespace hm
{
	class NodeQuantityOfMotion : public NodeUnthreaded
	{
	public:
		NodeQuantityOfMotion(Node::Params params, std::string className = "NodeQuantityOfMotion");

	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step();

	private:
		double mPrevTimestamp;

		InletPtr mInletUserIndex;
		InletPtr mInletDepth;
		OutletPtr mOutlet;
		OutletPtr mOutletNormalizedDepth;
		OutletPtr mOutletMeanDistance;
		OutletPtr mOutletMeanDifference;
		OutletPtr mOutletDifference;

		int mNormalizationLimitWindow;
		boost::circular_buffer<uint16_t> mNormalizationMinWindow;
		boost::circular_buffer<uint16_t> mNormalizationMaxWindow;

		int mPrevNormalizedDepthsWindow;
		boost::circular_buffer<cv::Mat> mPrevNormalizedDepths;


	};
}

#include "FactoryNode.h"
hm_register_node(NodeQuantityOfMotion)