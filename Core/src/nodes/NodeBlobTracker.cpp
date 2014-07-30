#include "NodeBlobTracker.h"
#include "Inlet.h"
#include "Outlet.h"
#include "Image2d.h"
#include "ListPoint2d.h"
#include "ListValue.h"
#include "CinderOpenCV.h"

using namespace hm;
using namespace std;



NodeBlobTracker::NodeBlobTracker(Node::Params const& params, std::string const& className)
: NodeUnthreaded(params, className, "Blob tracker", "Track blobs in a 2D image")
, mTimestampOfLastData(-42.)
, mForceUpdate(false)
{
	mInlet = createInlet(IMAGE2D, "Input image", "Images received here will be analysed for blobs");
	mBlobOutlet = createOutlet(LIST_POINT2D, "Blob positions", "A list of 2D vectors holding the positions of blobs that are being tracked");
	mSizeOutlet = createOutlet(LIST_VALUE, "Blob sizes", "A list of values holding the size of the blobs being tracked");
	
	// TODO: Implement colour parameter
	mBlobParams.filterByColor = false;
	
	addFilterParams(mBlobParams.filterByArea, mBlobParams.minArea, mBlobParams.maxArea, "area", 0, 10000);
	addFilterParams(mBlobParams.filterByCircularity, mBlobParams.minCircularity, mBlobParams.maxCircularity, "circularity", 0, 10);
	addFilterParams(mBlobParams.filterByInertia, mBlobParams.minInertiaRatio, mBlobParams.maxInertiaRatio, "inertia ratio", 0, 100);
	addFilterParams(mBlobParams.filterByConvexity, mBlobParams.minConvexity, mBlobParams.maxConvexity, "convexity", 0, 10000);
	
	for (ParameterPtr p: parameters())
	{
		p->addNewExternalValueCallback([this]() {
			mForceUpdate = true;
		});
	}

}

void NodeBlobTracker::addFilterParams(bool& filterOnOff, float& filterMin, float& filterMax, char const* name_, double softMin, double softMax)
{
	string name(name_);
	auto pOnOff = addParameter("Filter by "+name, &filterOnOff);
	auto pMin = addParameter("Min "+name, &filterMin);
	pMin->setBounds(0, 99999999, softMin, softMax);
	auto pMax = addParameter("Max "+name, &filterMax);
	pMax->setBounds(0, 99999999, softMin, softMax);
	function<void()> adjustVisibilityCallback = [=]() {
		pMin->setVisible(pOnOff->lastValue());
		pMax->setVisible(pOnOff->lastValue());
	};
	pOnOff->addNewExternalValueCallback(adjustVisibilityCallback);
	adjustVisibilityCallback();
}


NodePtr NodeBlobTracker::create(Node::Params params) const
{
	return NodePtr(new NodeBlobTracker(params));
}


void NodeBlobTracker::step()
{
	Data data = mForceUpdate? mInlet->data() : mInlet->dataIfNewerThan(mTimestampOfLastData);
	mTimestampOfLastData = data.timestamp();
	if (!data.isNull() && data.isImage2d())
	{
		assert(data.asImage2d().value.rows && data.asImage2d().value.cols);
		cv::SimpleBlobDetector blobDetector(mBlobParams);
		vector<cv::KeyPoint> keyPoints;
		blobDetector.detect(data.asImage2d().value, keyPoints);
		ListPoint2d blobs(mTimestampOfLastData);
		ListValue sizes(mTimestampOfLastData);
		for (cv::KeyPoint const& k: keyPoints)
		{
			blobs.value.push_back(Point2d(ci::fromOcv(k.pt), mTimestampOfLastData));
			sizes.value.push_back(Value(k.size, mTimestampOfLastData));
		}
		Data blobData(blobs);
		Data sizeData(sizes);
		mBlobOutlet->outputNewData(blobData);
		mSizeOutlet->outputNewData(sizeData);
	}
	mForceUpdate = false;
}