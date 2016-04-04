#include "nodes/NodeDepthStats.h"
#include "Inlet.h"
#include "Outlet.h"
#include "Data.h"
#include "Point3d.h"

using namespace hm;

NodeDepthStats::NodeDepthStats(Node::Params params, std::string const& className)
: NodeUnthreaded(params, className, "Depth stats", "Measures features of the closest user from a depth image, including position and size. Data will only be outputted if both a depth image and user index image is received.")
, mMinNumUserPixels(5)
{
	mDepthInlet = createInlet(IMAGE2D, "Depth image", "Input a depth image received from, e.g., a Kinect.");
	mUserIndexInlet = createInlet(IMAGE2D, "User index image", "Input a user index image received from, e.g., a Kinect.");

	mUserPositionOutlet = createOutlet(POINT3D, "User position", "The position of the nearest user to the camera is outputted as a 3D Vector (currently measured in camera space). This should be a 16 bit image.");
	mUserSizeOutlet = createOutlet(POINT3D, "User size", "The size of the nearest user is outputted as a 3D vector (currently measured in camera space). This should be an 8 bit image.");

	addParameter("Min user pixels", &mMinNumUserPixels, "The minimum number of pixels a user must occupy to be considered a user. This helps to filter out small objects that have erroneously been classed as a user.");
}

NodePtr NodeDepthStats::create(Node::Params params) const
{
	return NodePtr(new NodeDepthStats(params));
}

void NodeDepthStats::step()
{
	Data depthData = mDepthInlet->data();
	Data userData = mUserIndexInlet->data();
	if (depthData.isImage2d() && userData.isImage2d())
	{
		Image2d depthImage = depthData.asImage2d();
		Image2d userImage = userData.asImage2d();
		if (depthData.timestamp() > mLastTimestamp && depthImage.value.size() == userImage.value.size())
		{
			mLastTimestamp = depthData.timestamp();
			// ensure the depth image is of type CV_16U and user of type CV_8U
			if (depthImage.value.depth() != CV_16U)
			{
				cv::Mat tmp(depthImage.value.rows, depthImage.value.cols, CV_MAKE_TYPE(CV_16U, depthImage.value.channels()));
				depthImage.value.convertTo(tmp, tmp.type());
				depthImage.value = tmp;
			}
			if (userImage.value.depth() != CV_8U)
			{
				cv::Mat tmp(userImage.value.rows, userImage.value.cols, CV_MAKE_TYPE(CV_8U, userImage.value.channels()));
				userImage.value.convertTo(tmp, tmp.type());
				userImage.value = tmp;
			}

			// count the number of pixels per user
			std::vector<int> userPixelCount(256, 0);
			for (auto it = userImage.value.begin<uint8_t>(); it != userImage.value.end<uint8_t>(); ++it)
			{
				assert(*it < 256);
				userPixelCount[*it]++;
			}
			// assume user index above or equal to 250 is background
			// iterate over the depth image and calculate the mean distance of any user with more than mMinNumUserPixels
			std::vector<float> userDistanceSum(250, 0.f); ///< Sum of depth values for users indexed by user index
			std::set<int> userIndexes; ///< Indexes of users being considered
			{
				auto it_depth(depthImage.value.begin<uint16_t>()), end_depth(depthImage.value.end<uint16_t>());
				auto it_user(userImage.value.begin<uint8_t>()), end_user(userImage.value.end<uint8_t>());
				for (; it_depth != end_depth; ++it_depth, ++it_user)
				{
					assert(it_user != end_user);
					assert(*it_user < userPixelCount.size());
					if (*it_user < 250 && userPixelCount[*it_user] > mMinNumUserPixels)
					{
						userIndexes.insert(*it_user);
						userDistanceSum[*it_user] += *it_depth;
					}
				}
			}
			int nearestUserIndex(-42);
			float nearestUserMeanDistance = std::numeric_limits<float>::max();
			for (auto it = userIndexes.begin(); it != userIndexes.end(); ++it)
			{
				assert(*it < 250);
				float meanDistance = userDistanceSum[*it] / userPixelCount[*it];
				if (meanDistance < nearestUserMeanDistance)
				{
					nearestUserIndex = *it;
					nearestUserMeanDistance = meanDistance;
				}
			}
			// we now have our nearest user if any. If we do have one then calculate the stats.
			if (nearestUserIndex >= 0)
			{
				ci::Vec3f minPos(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
				ci::Vec3f maxPos(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
				ci::Vec3f userPositionSum;
				auto it_depth(depthImage.value.begin<uint16_t>()), end_depth(depthImage.value.end<uint16_t>());
				auto it_user(userImage.value.begin<uint8_t>()), end_user(userImage.value.end<uint8_t>());
				int const width(depthImage.value.cols), height(depthImage.value.rows);
				for (int i = 0; i != width*height; ++it_depth, ++it_user, ++i)
				{
					assert(it_user != end_user);
					assert(it_depth != end_depth);
					if (*it_user == nearestUserIndex)
					{
						ci::Vec3f const pos(i / width, i % width, *it_depth);
						minPos.x = std::min<float>(minPos.x, pos.x);
						minPos.y = std::min<float>(minPos.y, pos.y);
						minPos.z = std::min<float>(minPos.z, pos.z);
						maxPos.x = std::max<float>(maxPos.x, pos.x);
						maxPos.y = std::max<float>(maxPos.y, pos.y);
						maxPos.z = std::max<float>(maxPos.z, pos.z);
						userPositionSum += pos;
					}
				}
				assert(it_user == end_user);
				assert(it_depth == end_depth);
				ci::Vec3f userPositionMean = userPositionSum / userPixelCount[nearestUserIndex];
				ci::Vec3f userSize = maxPos - minPos;
				Data meanPositionData(Point3d(userPositionMean, depthImage.timestamp, nearestUserIndex, depthImage.sceneMeta));
				mUserPositionOutlet->outputNewData(meanPositionData);
				Data sizeData(Point3d(userSize, depthImage.timestamp, nearestUserIndex, depthImage.sceneMeta));
				mUserSizeOutlet->outputNewData(sizeData);
			}
		}
	}
}