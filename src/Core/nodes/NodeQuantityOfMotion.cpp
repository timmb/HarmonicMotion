#include "nodes/NodeQuantityOfMotion.h"
#include "Inlet.h"
#include "Outlet.h"
#include "CinderOpenCV.h"
#include <numeric>

using namespace hm;
using namespace std;

NodeQuantityOfMotion::NodeQuantityOfMotion(Node::Params params, std::string className)
: NodeUnthreaded(params, className, "Quantity of Motion", "Calculates the amount of motion detected from a user index image.")
, mPrevTimestamp(-42.)
, mNormalizationLimitWindow(3)
, mPrevNormalizedDepthsWindow(3)
{
	mInletUserIndex = createInlet(IMAGE2D, "User index image", "Provide a user index image from a depth camera (e.g. Kinect) here.");
	mInletDepth = createInlet(IMAGE2D, "Depth image", "Provide a depth image matching the user index image provided in the above inlet.");
	mOutlet = createOutlet(VALUE, "Quantity of Motion", "The Quantity of Motion detected in the image is outputted here.");
	mOutletNormalizedDepth = createOutlet(IMAGE2D, "Normalized depth");
	mOutletMeanDistance = createOutlet(VALUE, "Mean distance");
	mOutletMeanDifference = createOutlet(VALUE, "Mean difference");
	mOutletDifference = createOutlet(IMAGE2D, "Difference");

	{
		auto p = addParameter("Normalization limit window (frames)", &mNormalizationLimitWindow, "How many frames to average over when calculating the minimum and maximum distance of the user for normalization calculations");
		p->setBounds(1, 100000, 1, 20);
		p->addNewExternalValueCallback([&]() {
			mNormalizationMaxWindow.set_capacity(mNormalizationLimitWindow);
			mNormalizationMinWindow.set_capacity(mNormalizationLimitWindow);
		});
		mNormalizationMaxWindow.set_capacity(mNormalizationLimitWindow);
		mNormalizationMinWindow.set_capacity(mNormalizationLimitWindow);
	}

	{
		auto p = addParameter("Normalized window (frames)", &mPrevNormalizedDepthsWindow, "How many frames to average over when calculating the difference image.");
		p->setBounds(1, 5000, 1, 20);
		p->addNewExternalValueCallback([&]() {
			mPrevNormalizedDepths.set_capacity(mPrevNormalizedDepthsWindow);
		});
		mPrevNormalizedDepths.set_capacity(mPrevNormalizedDepthsWindow);
	}
}


NodePtr NodeQuantityOfMotion::create(Node::Params params) const
{
	return NodePtr(new NodeQuantityOfMotion(params));
}


void NodeQuantityOfMotion::step()
{
	if (mInletUserIndex->dataTimestamp() > mPrevTimestamp)
	{
		Data userData = mInletUserIndex->data();
		Data depthData = mInletDepth->data();
		if (userData.type() == IMAGE2D && depthData.type() == IMAGE2D 
			&& userData.asImage2d().value.size() == depthData.asImage2d().value.size())
		{
			Image2d userImage = userData.asImage2d();
			Image2d depthImage = depthData.asImage2d();
			cv::Mat& depth = depthImage.value;
			cv::Mat& user = userImage.value;
			cv::Mat userMask = userImage.value < 250;
			
			// force depth to be cv_16u and user cv_8u in case they're
			// not for some reason
			if (depth.depth() != CV_16U)
			{
				cv::Mat tmp(depth.rows, depth.cols, CV_MAKE_TYPE(CV_16U, depth.channels()));
				depth.convertTo(tmp, tmp.type());
				depth = tmp;
			}
			if (user.depth() != CV_8U)
			{
				cv::Mat tmp(user.rows, user.cols, CV_MAKE_TYPE(CV_8U, user.channels()));
				user.convertTo(tmp, tmp.type());
				user = tmp;
			}

			// Mask depth based on user and normalize it
			uint16_t depthMin(numeric_limits<uint16_t>::max());
			uint16_t depthMax(numeric_limits<uint16_t>::min());
			{
				auto depth_it = depth.begin<uint16_t>();
				auto user_it = user.begin<uint8_t>();
				for ( ; depth_it != depth.end<uint16_t>(); ++depth_it, ++user_it)
				{
					// assume user indexes above 250 are classified as background
					int const nonBackgroundLimit = numeric_limits<uint16_t>::max() - 1;
					if (*user_it < uint8_t(250))
					{
						depthMin = min(depthMin, *depth_it);
						depthMax = max(depthMax, *depth_it);
					}
					else
					{
						// set background pixels to be maximum value
						*depth_it = 0;
					}
					
				}
			}
			// To compensate for noise, we take a moving average of the min and max when
			// normalizing
			mNormalizationMaxWindow.push_back(depthMax);
			mNormalizationMinWindow.push_back(depthMin);
			assert(mNormalizationMaxWindow.size() <= mNormalizationLimitWindow && mNormalizationMinWindow.size() <= mNormalizationLimitWindow);
			uint16_t normMin = accumulate<boost::circular_buffer<uint16_t>::iterator, uint16_t>(mNormalizationMinWindow.begin(), mNormalizationMinWindow.end(), 0) / mNormalizationMinWindow.size();
			uint16_t normMax = accumulate<boost::circular_buffer<uint16_t>::iterator, uint16_t>(mNormalizationMaxWindow.begin(), mNormalizationMaxWindow.end(), 0) / mNormalizationMaxWindow.size();
			uint16_t range = (normMax - normMin);
			// to avoid clipping when the values exceed the averaged min/max limits, we double the size of our normalization bounds
			uint16_t halfRange = range / uint16_t(2);
			normMax = numeric_limits<uint16_t>::max() - normMax > halfRange ? normMax + halfRange : numeric_limits<uint16_t>::max();
			normMin = normMin > halfRange ? normMin - halfRange : uint16_t(0);
			range *= 2;
			depth -= normMin;

			cv::Mat normalizedDepth;
			depth.convertTo(normalizedDepth, CV_32F, 1.f / range);

			//cv::Mat normalizedDepth = cv::Mat(depth.rows, depth.cols, CV_8U, cv::Scalar(0));
			//cv::normalize(depth, normalizedDepth, std::numeric_limits<uint8_t>::max(), 0, CV_MINMAX, CV_8U, userMask);

			Data normalizedDepthData(Image2d(normalizedDepth, userImage.timestamp, userImage.id, depthImage.sceneMeta));
			mOutletNormalizedDepth->outputNewData(normalizedDepthData);

			if (!mPrevNormalizedDepths.empty() && (normalizedDepth.size() != mPrevNormalizedDepths.back().size()
				|| normalizedDepth.type() != mPrevNormalizedDepths.back().type()))
			{
				mPrevNormalizedDepths.clear();
			}
			//if (normalizedDepth.size() == mPrevNormalizedDepth.value.size()
				//|| normalizedDepth.type() == mPrevNormalizedDepth.value.type())
			{
				cv::Mat meanPrev = std::accumulate<boost::circular_buffer<cv::Mat>::const_iterator, cv::Mat>(mPrevNormalizedDepths.begin(), mPrevNormalizedDepths.end(), cv::Mat::zeros(normalizedDepth.size(), normalizedDepth.type()));
				if (!mPrevNormalizedDepths.empty())
					meanPrev /= mPrevNormalizedDepths.size();
				cv::Mat diff = abs(normalizedDepth - meanPrev);
				cv::Mat tmp(diff.size(), diff.type());
				cv::medianBlur(diff, tmp, 3);
				diff = tmp;
				Data diffData(Image2d(diff, userImage.timestamp, userImage.id, userImage.sceneMeta));
				mOutletDifference->outputNewData(diffData);
				float meanDistance = cv::mean(normalizedDepth, userMask)[0];
				Data meanDistanceData(Value(meanDistance, userImage.timestamp, userImage.id, depthImage.sceneMeta));
				mOutletMeanDistance->outputNewData(meanDistanceData);
				float meanDifference = cv::mean(diff)[0];
				Data meanDifferenceData(Value(meanDifference, userImage.timestamp, userImage.id, depthImage.sceneMeta));
				mOutletMeanDifference->outputNewData(meanDifferenceData);
				double qom = meanDifference * meanDistance * meanDistance;
				Value v(qom, userImage.timestamp, userImage.id, depthImage.sceneMeta);
				Data data(v);
				mOutlet->outputNewData(data);
			}
			mPrevNormalizedDepths.push_back(normalizedDepth);
			//mPrevNormalizedDepth = Image2d(normalizedDepth, userImage.timestamp, userImage.id, depthImage.sceneMeta);
		}
		mPrevTimestamp = userData.timestamp();
	}
}

