#pragma once

#include "NodeUnthreaded.h"
#include "Image2d.h"

namespace hm
{
	class NodeOpticalFlow : public NodeUnthreaded
	{
	public:
		NodeOpticalFlow(Params const& params, std::string const& className="NodeOpticalFlow");

		/// input will be set to empty matrix by this function
		/// If input is different size or type from previous input
		/// then output will be an empty matrix
		cv::Mat process(cv::Mat& input);

	protected:
		virtual void step() override;
		virtual NodePtr create(Params params) const override;

	private:
		InletPtr mInlet;
		OutletPtr mOutlet;
		double mLastTimestamp;
		
		cv::Mat mPrevImage;

		// Parameters
		double mPyrScale;
		int mLevels;
		int mWindowSize;
		int mIterations;
		int mPolyN;
		double mPolySigma;
		bool mUseGaussianFilter;
	};
}
#include "FactoryNode.h"
hm_register_node(NodeOpticalFlow)