#include "nodes/NodeBackgroundSubtractor.h"
#include "Inlet.h"
#include "Outlet.h"
#include "Parameter.h"

using namespace hm;

NodeBackgroundSubtractor::NodeBackgroundSubtractor(Params const& params, std::string const& className)
: NodeUnthreaded(params, className, "Background subtractor", "Remove the background from a stream of images.")
, mBgSubtractorMog2(new cv::BackgroundSubtractorMOG2)
, mLearningRate(0.1)
, nmixtures(2)
, backgroundRatio(0.9)
, varThresholdGen(9)
, fVarInit(1)
, fVarMin(0)
, fVarMax(100)
, fCT(0.05)
, nShadowDetection(127)
, fTau(1.0)
, mFrameType(-42)
{
	mInlet = createInlet(IMAGE2D, "Input images", "Images received here will have the background subtracted from them based on past images.");
	mOutletForeground = createOutlet(IMAGE2D, "Foreground", "The received image with the background pixels set to zero (black) will be outputted here.");
	mOutletForegroundMask = createOutlet(IMAGE2D, "Foreground mask", "An 8 bit one channel mask with foreground pixels as 255 (white), shadow as 127 (grey) and background as black (0) will be outputted here.");

	mBgSubtractorMog2->set("nmixtures", nmixtures);
	mBgSubtractorMog2->set("backgroundRatio", backgroundRatio);
	mBgSubtractorMog2->set("varThresholdGen", varThresholdGen);
	mBgSubtractorMog2->set("fVarInit", fVarInit);
	mBgSubtractorMog2->set("fVarMin", fVarMin);
	mBgSubtractorMog2->set("fVarMax", fVarMax);
	mBgSubtractorMog2->set("fCT", fCT);
	mBgSubtractorMog2->set("nShadowDetection", nShadowDetection);
	mBgSubtractorMog2->set("fTau", fTau);

	addParameter("learning rate", &mLearningRate);
	addParameter("nmixtures", &nmixtures)->addNewExternalValueCallback([&](){ mBgSubtractorMog2->set("nmixtures", nmixtures); });
	addParameter("backgroundRatio", &backgroundRatio)->addNewExternalValueCallback([&](){ mBgSubtractorMog2->set("backgroundRatio", backgroundRatio); });
	addParameter("varThresholdGen", &varThresholdGen)->addNewExternalValueCallback([&](){ mBgSubtractorMog2->set("varThresholdGen", varThresholdGen); });
	addParameter("fVarInit", &fVarInit)->addNewExternalValueCallback([&](){ mBgSubtractorMog2->set("fVarInit", fVarInit); });
	addParameter("fVarMin", &fVarMin)->addNewExternalValueCallback([&](){ mBgSubtractorMog2->set("fVarMin", fVarMin); });
	addParameter("fVarMax", &fVarMax)->addNewExternalValueCallback([&](){ mBgSubtractorMog2->set("fVarMax", fVarMax); });
	addParameter("fCT", &fCT)->addNewExternalValueCallback([&](){ mBgSubtractorMog2->set("fCT", fCT); });
	addParameter("nShadowDetection", &nShadowDetection)->addNewExternalValueCallback([&](){ mBgSubtractorMog2->set("nShadowDetection", nShadowDetection); });
	addParameter("fTau", &fTau)->addNewExternalValueCallback([&](){ mBgSubtractorMog2->set("fTau", fTau); });
}


NodePtr NodeBackgroundSubtractor::create(Params params) const
{
	return NodePtr(new NodeBackgroundSubtractor(params));
}

void NodeBackgroundSubtractor::step()
{
	if (mInlet->dataTimestamp() > mLastTimestamp)
	{
		Data d = mInlet->data();
		if (d.isImage2d())
		{
			Image2d& image = d.asImage2d();
			if (image.value.size() != mFrameSize || image.value.type() != mFrameType)
			{
				mFrameSize = image.value.size();
				mFrameType = image.type();
				mBgSubtractorMog2->initialize(image.value.size(), image.value.type());
			}
			assert(mFrameType != -42);
			cv::Mat fgMask;
			(*mBgSubtractorMog2)(image.value, fgMask, mLearningRate);
			if (mOutletForeground->numInlets() > 0)
			{
				Image2d maskedOut(cv::Mat(image.value.size(), image.value.type(), cv::Scalar(0)), image.timestamp, image.id, image.sceneMeta);
				image.value.copyTo(maskedOut.value, fgMask);
				Data out(maskedOut);
				mOutletForeground->outputNewData(out);
			}
			if (mOutletForegroundMask->numInlets() > 0)
			{
				Data out(Image2d(fgMask, image.timestamp, image.id, image.sceneMeta, Image2d::MASK));
				mOutletForegroundMask->outputNewData(out);
			}
		}
		mLastTimestamp = d.timestamp();
	}
}