#include "nodes/NodeKinectKinectSdk.h"
#include "Kinect2.h"
#include <boost/thread/thread.hpp>
#include "SceneMeta.h"
#include "Outlet.h"
#include "Image2d.h"
#include "Scene3d.h"
#include "Kinect.h"

using namespace hm;
using boost::unique_lock;
using boost::shared_lock;
using boost::shared_mutex;
using boost::upgrade_mutex;
using boost::upgrade_lock;
using boost::upgrade_to_unique_lock;
using namespace std;

hm::Joint kcbJointToHm(JointType x)
{
	switch (x)
	{
	case JointType_SpineBase:
		return SPINE_BASE;
	case JointType_SpineMid:
		return SPINE_MID;
	case JointType_Neck:
		return NECK;
	case JointType_Head:
		return HEAD;
	case JointType_ShoulderLeft:
		return LEFT_SHOULDER;
	case JointType_ElbowLeft:
		return LEFT_ELBOW;
	case JointType_WristLeft:
		return LEFT_WRIST;
	case JointType_HandLeft:
		return LEFT_HAND;
	case JointType_ShoulderRight:
		return RIGHT_SHOULDER;
	case JointType_ElbowRight:
		return RIGHT_ELBOW;
	case JointType_WristRight:
		return RIGHT_WRIST;
	case JointType_HandRight:
		return RIGHT_HAND;
	case JointType_HipLeft:
		return LEFT_HIP;
	case JointType_KneeLeft:
		return LEFT_KNEE;
	case JointType_AnkleLeft:
		return LEFT_ANKLE;
	case JointType_FootLeft:
		return LEFT_FOOT;
	case JointType_HipRight:
		return RIGHT_HIP;
	case JointType_KneeRight:
		return RIGHT_KNEE;
	case JointType_AnkleRight:
		return RIGHT_ANKLE;
	case JointType_FootRight:
		return RIGHT_FOOT;
	case JointType_SpineShoulder:
		return SPINE_TOP;
	case JointType_HandTipLeft:
		return LEFT_HAND_TIP;
	case JointType_ThumbLeft:
		return LEFT_THUMB;
	case JointType_HandTipRight:
		return RIGHT_HAND_TIP;
	case JointType_ThumbRight:
		return RIGHT_THUMB;
	default:
		assert(false);
		return -1;
	}
}

NodeKinect::NodeKinect(Params const& params, std::string const& className)
: NodeUnthreaded(params, className, "Kinect (Microsoft SDK 2)", "Represents a Kinect One camera accessed using the Microsoft SDK v2")
, mIsRunning(false)
, mTimeOfLastAttemptToOpenKinect(-42.)
, mSceneMeta(SceneMeta::defaultSceneMeta())
, mEnableMirroring(true)
{
	mSceneOutlet = createOutlet(SCENE3D, "Scene", "The current 3D scene (a set of 3D skeletons) are outputted here.");
	mDepthOutlet = createOutlet(IMAGE2D, "Depth image", "The current image received from the depth sensor as a 2D image (further distances have higher pixel values).");
	mBodyIndexOutlet = createOutlet(IMAGE2D, "User index image", "A one channel image with each pixel set to the user ID of any tracked users, a value higher than the number of tracked users if the pixel is part of the background.");
	mInfraredOutlet = createOutlet(IMAGE2D, "Infrared image", "The infrared image received from the Kinect");

	addParameter("Mirror output", &mEnableMirroring, "This outputs data mirrored in the x axis which will cause your movements to match the image if you are facing the camera and screen simultaneously. Default is enabled.");
}

NodeKinect::~NodeKinect()
{
	assert(!mIsRunning);
}

NodePtr NodeKinect::create(Node::Params params) const
{
	return NodePtr(new NodeKinect(params));
}

void NodeKinect::start()
{
	unique_lock<shared_mutex> lock(mIsRunningMutex);
	mIsRunning = true;
}

void NodeKinect::step()
{
	assert(mIsRunning);
	if (mKinect == nullptr)
	{
		double t = elapsedTime();
		if (t - mTimeOfLastAttemptToOpenKinect > 5)
		{
			try
			{
				mKinect = Kinect2::Device::create();
				mKinect->connectBodyEventHandler(&NodeKinect::callbackBodyFrame, this);
				mKinect->connectDepthEventHandler(&NodeKinect::callbackDepthFrame, this);
				mKinect->connectBodyIndexEventHandler(&NodeKinect::callbackBodyIndexFrame, this);
				mKinect->connectInfraredEventHandler(&NodeKinect::callbackInfraredFrame, this);
				mKinect->start();
			}
			catch (Kinect2::Device::ExcDeviceNotAvailable const& e)
			{
				hm_warning(string("Kinect unavailable: ") + e.what());
				mKinect = nullptr;
			}
			catch (Kinect2::Device::ExcDeviceOpenFailed const& e)
			{
				hm_warning(string("Failed to open Kinect: ") + e.what());
				mKinect = nullptr;
			}
		}
	}

	if (mKinect != nullptr)
	{
		mKinect->update();
	}
}

void NodeKinect::stop()
{
	unique_lock<shared_mutex> lock(mIsRunningMutex);
	mIsRunning = false;
	mKinect->stop();
	mKinect->disconnectBodyEventHandler();
	mKinect->disconnectDepthEventHandler();
	mKinect->disconnectBodyIndexEventHandler();
	mKinect->disconnectInfraredEventHandler();
	mKinect = nullptr;
}

void NodeKinect::callbackDepthFrame(Kinect2::DepthFrame const& frame)
{
	upgrade_lock<upgrade_mutex> lock(mSceneMetaMutex);
	if (mSceneMeta == SceneMeta::defaultSceneMeta())
	{
		upgrade_to_unique_lock<upgrade_mutex> lock2(lock);
		SceneMeta* s(new SceneMeta());
		s->cameraFov = frame.getFovHorizontal();
		s->viewportSize = frame.getSize();
		mSceneMeta = SceneMetaPtr(s);
	}
	if (mDepthOutlet->numInlets() > 0)
	{
		Image2d image(frame.getChannel(), elapsedTime(), 0, mSceneMeta);
		image.role = Image2d::DEPTH;
		if (!mEnableMirroring)
		{
			cv::flip(image.value, image.value, 1);
		}
		Data data(image);
		mDepthOutlet->outputNewData(data);
	}
}

void NodeKinect::callbackBodyFrame(Kinect2::BodyFrame const& frame)
{
	if (mSceneOutlet->numInlets() > 0)
	{
		double t = elapsedTime();
		SceneMetaPtr sceneMeta;
		{
			upgrade_lock<upgrade_mutex> lock(mSceneMetaMutex);
			sceneMeta = mSceneMeta;
			if (sceneMeta->numUsers != frame.getBodies().size())
			{
				SceneMeta* p_sceneMeta = new SceneMeta(*sceneMeta);
				p_sceneMeta->numUsers = frame.getBodies().size();
				sceneMeta = SceneMetaPtr(p_sceneMeta);
				upgrade_to_unique_lock<upgrade_mutex> lock2(lock);
				mSceneMeta = sceneMeta;
			}
		}
		Scene3d scene(elapsedTime(), 0, sceneMeta);
		for (Kinect2::Body const& b : frame.getBodies())
		{
			if (b.isTracked())
			{
				Skeleton3d s(t, b.getId(), sceneMeta);
				for (auto const& pair : b.getJointMap())
				{
					Joint id = kcbJointToHm(pair.first);
					Kinect2::Body::Joint const& j = pair.second;
					ci::Vec3f worldPosition = j.getPosition();
					ci::Vec3f projectedPosition = ci::Vec3f(mKinect->mapCameraToDepth(worldPosition), worldPosition.z);
					if (mEnableMirroring)
					{
						worldPosition.x = -worldPosition.x;
					}
					// For some reason the coordinates come out of the projection with x going left, y going down and z going forwards
					// (i.e. left handed). We swap the x to get normal image-style right handed coordinates (x right, y down, z forwards).
					projectedPosition.x = sceneMeta->viewportSize.x - projectedPosition.x;
					s.joint(id) = Point3d(worldPosition, t, id, sceneMeta);
					s.jointConfidence(id) = j.getTrackingState() / 2.f;
					s.jointProjective(id) = Point3d(projectedPosition, t, id, sceneMeta);
				}
				scene.value.push_back(s);
			}
		}
		Data data(scene);
		mSceneOutlet->outputNewData(data);
	}
}


void NodeKinect::callbackBodyIndexFrame(Kinect2::BodyIndexFrame const& frame)
{
	if (mBodyIndexOutlet->numInlets() > 0)
	{
		double t = elapsedTime();
		Image2d image(frame.getChannel(), t, 0, mSceneMeta);
		image.role = Image2d::USER_INDEX;
		if (!mEnableMirroring)
		{
			cv::flip(image.value, image.value, 1);
		}
		Data data(image);
		mBodyIndexOutlet->outputNewData(data);
	}
}



void NodeKinect::callbackInfraredFrame(Kinect2::InfraredFrame const& frame)
{
	if (mInfraredOutlet->numInlets() > 0)
	{
		double t = elapsedTime();
		Image2d image(frame.getChannel(), t, 0, mSceneMeta);
		if (!mEnableMirroring)
		{
			cv::flip(image.value, image.value, 1);
		}
		Data data(image);
		mInfraredOutlet->outputNewData(data);
	}
}