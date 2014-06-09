#include "NodeCamera.h"
#include "CinderOpenCV.h"
#include "Outlet.h"

using namespace hm;
using namespace std;
using namespace ci;

NodeCamera::NodeCamera(Node::Params const& params, std::string const& className)
: NodeUnthreaded(params, className)
, mCurrentDevice(-42)
, mRequestedDevice(0)
, mRequestedWidth(640)
, mActualWidth(0)
, mRequestedHeight(480)
, mActualHeight(0)
{
	mRequestedDeviceParameter = addParameter("Source", &mRequestedDevice);
	assert(mRequestedDeviceParameter);
	addParameter("Requested width", &mRequestedWidth)->softMax = 40960;
	addParameter("Requested height", &mRequestedHeight)->softMax = 40960;
	addParameter("Actual width", &mActualWidth)->softMax = 40960;
	addParameter("Actual height", &mActualHeight)->softMax = 40960;
	updateCurrentDevice();
	
	mOutlet = createOutlet(IMAGE2D, "Camera images", "2D images will be outputted here as they are received from the camera");
}

void NodeCamera::updateCurrentDevice()
{
	vector<Capture::DeviceRef> devices = Capture::getDevices();
	mRequestedDeviceParameter->softMin = mRequestedDeviceParameter->hardMin = 0;
	mRequestedDeviceParameter->softMax = mRequestedDeviceParameter->hardMax	= std::max<int>(0, devices.size() -1);
	mRequestedDevice = max(0, min<int>(devices.size()-1, mRequestedDevice));
	if (mRequestedDevice >= devices.size())
	{
		if (mCapture)
		{
			mCapture->stop();
			mActualWidth = mActualHeight = 0;
		}
		mCapture = nullptr;
	}
	else
	{
		mCapture = Capture::create(mRequestedWidth, mRequestedHeight, devices[mRequestedDevice]);
		if (mCapture)
		{
			mCurrentDevice = mRequestedDevice;
			mActualWidth = mCapture->getWidth();
			mActualHeight = mCapture->getHeight();
			mCapture->start();
		}
	}
}

NodePtr NodeCamera::create(Node::Params params) const
{
	return NodePtr(new NodeCamera(params));
}

void NodeCamera::start()
{
	
}

void NodeCamera::step()
{
	if (mCurrentDevice != mRequestedDevice)
	{
		updateCurrentDevice();
	}
	if (mCapture && mCapture->isCapturing())
	{
		Surface8u surface = mCapture->getSurface();
		if (surface)
		{
			Data data(Image2d(surface, elapsedTime()));
			mOutlet->outputNewData(data);
		}
	}
	
}

void NodeCamera::stop()
{
	
}


