#include "nodes/NodeOpticalFlow.h"
#include "NodeUnthreaded.h"
#include "Data.h"
#include "Inlet.h"
#include "Outlet.h"
#include "CinderOpenCV.h"
#include "Parameter.h"
//#include "opencv2/video/tracking.hpp"

using namespace hm;

NodeOpticalFlow::NodeOpticalFlow(Params const& params, std::string const& className)
: NodeUnthreaded(params, className, "Optical flow", "Calculates motion field from an image")
, mLastTimestamp(-42.)
, mPyrScale(0.5)
, mLevels(4)
, mWindowSize(25)
, mIterations(5)
, mPolyN(7)
, mPolySigma(1.5)
, mUseGaussianFilter(false)
{
	mInlet = createInlet(IMAGE2D, "Image input", "Optical flow will be calculated for successive images received here using the Gunnar Farneback algorithm. Images must match in size and type. If image has multiple channels, only the first channel will be used in calculations.");
	mOutlet = createOutlet(IMAGE2D, "Optical flow", "The optical motion between successive images is calculated and outputted here as a two channel image of 32 bit floats. The first channel is motion in the x axis, the second channel is motion in the y axis.");

	addParameter("Pyramid scale", &mPyrScale, "The image scale to build pyramids for each image. 0.5 means a classical pyramid, where each next layer is twice smaller than the previous one.")->setBounds(0., 0.999, 0., 0.999);
	addParameter("Levels", &mLevels, "Number of pyramid layers, including the initial image. 1 means no extra layers are created and only the original images are used.")->setBounds(1, 999999999, 1, 20);
	addParameter("Window size", &mWindowSize, "Averaging window size. larger values increase the algorithm robustness to image noise and give more chances for fast motion detection, but yield more blurred motion field.")->setBounds(1, 999999999, 1, 200);
	addParameter("Iterations", &mIterations, "Number of iterations the algorithm does at each pyramid level.")->setBounds(1, 500, 1, 10);
	addParameter("Poly N", &mPolyN, "Size of the pixel neighborhood used to find polynomial expansion in each pixel; larger values mean that the image will be approximated with smoother surfaces, yielding more robust algorithm and more blurred motion field, typically poly_n = 5 or 7.")->setBounds(1, 999, 1, 20);
	addParameter("Poly Sigma", &mPolySigma, "standard deviation of the Gaussian that is used to smooth derivatives used as a basis for the polynomial expansion; for poly_n = 5, you can set poly_sigma = 1.1, for poly_n = 7, a good value would be poly_sigma = 1.5.")->setBounds(0, 999999999, 0, 10);
	addParameter("Gaussian filter", &mUseGaussianFilter, "Use a Gaussian filter instead of a box filter when averaging. More accurate but slower. Window size should be set higher if using this.");


}

NodePtr NodeOpticalFlow::create(Params params) const
{
	return NodePtr(new NodeOpticalFlow(params));
}

cv::Mat NodeOpticalFlow::process(cv::Mat & in)
{
    cv::Mat grayInput = in;
    if (grayInput.channels() > 1)
    {
        cv::cvtColor(grayInput, grayInput, CV_BGR2GRAY);
    }

    cv::Mat flow;
    if (grayInput.type() == mPrevImage.type() && grayInput.size() == mPrevImage.size())
	{
		flow = cv::Mat(grayInput.size(), CV_32FC2);
		calcOpticalFlowFarneback(mPrevImage, grayInput, flow, mPyrScale, mLevels, mWindowSize, mIterations, mPolyN, mPolySigma, mUseGaussianFilter ? cv::OPTFLOW_FARNEBACK_GAUSSIAN : 0);
	}
	mPrevImage = grayInput;
	return flow;

}

void NodeOpticalFlow::step()
{
	if (mInlet->dataTimestamp() > mLastTimestamp)
	{
		Data data = mInlet->data();
		if (data.isImage2d())
		{
			Image2d image = data.asImage2d();
			cv::Mat flow = process(image.value);
			if (flow.rows > 0 && flow.cols > 0)
			{
				Image2d imageOut(flow, image.timestamp, image.id, image.sceneMeta);
				imageOut.role = Image2d::FLOW_FIELD;
				Data out(imageOut);
				mOutlet->outputNewData(out);
			}
		}
		mLastTimestamp = mInlet->dataTimestamp();
	}
}