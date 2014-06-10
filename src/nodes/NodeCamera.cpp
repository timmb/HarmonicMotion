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
	addParameter("Requested width", &mRequestedWidth)->setBounds(0, 999999, 0, 40960);
	addParameter("Requested height", &mRequestedHeight)->setBounds(0, 999999, 0, 40960);
	addParameter("Actual width", &mActualWidth)->setBounds(0, 999999, 0, 40960);
	addParameter("Actual height", &mActualHeight)->setBounds(0, 999999, 0, 40960);
	updateCurrentDevice();
	
	mOutlet = createOutlet(IMAGE2D, "Camera images", "2D images will be outputted here as they are received from the camera");
}

NodeCamera::~NodeCamera()
{
}

void NodeCamera::updateCurrentDevice()
{
	vector<Capture::DeviceRef> devices = Capture::getDevices(true);
	// max is equal to the number of devices as we also have device 0 as None
	vector<string> deviceNames;
	deviceNames.push_back("None");
	for (auto x: devices)
	{
		deviceNames.push_back(x->getName());
	}
	mRequestedDeviceParameter->setEnumerationLabels(deviceNames);
	
	mRequestedDevice = max(0, min<int>(devices.size(), mRequestedDevice));
	
	if (mRequestedDevice > devices.size() || mRequestedDevice == 0)
	{
		if (mCapture)
		{
			mCapture->stop();
			mActualWidth = mActualHeight = 0;
		}
		mCapture = nullptr;
		mCurrentDevice = 0;
	}
	else
	{
		mCapture = Capture::create(mRequestedWidth, mRequestedHeight, devices[mRequestedDevice-1]);
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
	if (mCapture && mCapture->isCapturing() && mCapture->checkNewFrame())
	{
		Surface8u surface = mCapture->getSurface();
		if (surface)
		{
			mActualWidth = surface.getWidth();
			mActualHeight = surface.getHeight();
			Data data(Image2d(surface, elapsedTime()));
			mOutlet->outputNewData(data);
		}
	}
	
}

void NodeCamera::stop()
{
	if (mCapture)
	{
		mCapture->stop();
	}
}


