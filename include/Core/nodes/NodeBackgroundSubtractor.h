#pragma once

#include "NodeUnthreaded.h"

namespace cv
{
	class BackgroundSubtractorMOG2;
}


namespace hm
{
	class NodeBackgroundSubtractor : public NodeUnthreaded
	{
	public:
		NodeBackgroundSubtractor(Params const& params, std::string const& className = "NodeBackgroundSubtractor");

	protected:
		virtual NodePtr create(Params params) const override;
		virtual void step() override;

	private:
		InletPtr mInlet;
		OutletPtr mOutletForeground;
		OutletPtr mOutletForegroundMask;

		double mLastTimestamp;

		enum MODEL_
		{
			MOG2,
			NUM_MODELS
		};
		typedef int Model;
		Model mModel;

		std::unique_ptr<cv::BackgroundSubtractorMOG2> mBgSubtractorMog2;
		cv::Size mFrameSize;
		int mFrameType;
		
		double mLearningRate;

		bool mFlowBsParamsChanged;
		int nmixtures;
		float backgroundRatio;
		float varThresholdGen;
		float fVarInit;
		float fVarMin;
		float fVarMax;
		float fCT;
		int nShadowDetection;
		float fTau;
	};
}
#include "FactoryNode.h"
hm_register_node(NodeBackgroundSubtractor)