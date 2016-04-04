
#ifdef HM_OPENNI

#include "nodes/NodeKinect.h"
#include "VOpenNIHeaders.h"
#include "Outlet.h"
#include "Scene3d.h"
#include <boost/thread/thread.hpp>
#include "SceneMeta.h"

using namespace hm;
using namespace V;
using namespace boost;

namespace
{
	// small closure to set a static variable before anything else is
	// instantiated.
	bool unused = [](){
		OpenNIDeviceManager::USE_THREAD = false;
		return false;
	}();
}

boost::shared_ptr<V::OpenNIDevice> NodeKinect::mDevice((nullptr));
boost::try_mutex NodeKinect::mDeviceMutex;

int jointToVBone(Joint jointId)
{
	switch (jointId)
	{
		case HEAD:
			return SKEL_HEAD;
		case NECK:
			return SKEL_NECK;
		case TORSO:
			return SKEL_TORSO;
		case LEFT_SHOULDER:
			return SKEL_LEFT_SHOULDER;
		case LEFT_ELBOW:
			return SKEL_LEFT_ELBOW;
		case LEFT_HAND:
			return SKEL_LEFT_HAND;
		case RIGHT_SHOULDER:
			return SKEL_RIGHT_SHOULDER;
		case RIGHT_ELBOW:
			return SKEL_RIGHT_ELBOW;
		case RIGHT_HAND:
			return SKEL_RIGHT_HAND;
		case LEFT_HIP:
			return SKEL_LEFT_HIP;
		case LEFT_KNEE:
			return SKEL_LEFT_KNEE;
		case LEFT_FOOT:
			return SKEL_LEFT_FOOT;
		case RIGHT_HIP:
			return SKEL_RIGHT_HIP;
		case RIGHT_KNEE:
			return SKEL_RIGHT_KNEE;
		case RIGHT_FOOT:
			return SKEL_RIGHT_FOOT;
		default:
			assert(false);
			return 0;
	}
}

NodeKinect::NodeKinect(Params const& params, std::string const& className)
: NodeThreaded(params, className, "Kinect (OpenNI)", "Receive skeleton data from a Kinect using the OpenNI/NITE libraries.")
, mSceneOutlet(nullptr)
, mMetadata(nullptr)
, mEnableScene(true)
, mEnableDepth(true)
{
	mSceneOutlet = createOutlet(SCENE3D, "3D Scene", "All user skeletons in 3D space");
	SceneMeta* metadata = new SceneMeta;
	metadata->cameraPos = ci::Vec3f(0,0,0);
	metadata->cameraDir = ci::Vec3f(0,0,1);
	metadata->cameraFov = 57;
	mMetadata = SceneMetaPtr(metadata);
	
}

NodeKinect::~NodeKinect()
{
//	std::cout << "~NodeKinect stopping"<<std::endl;;
	stopAndWait();
//	std::cout << "~NodeKinect stopped"<<std::endl;;
}

void NodeKinect::openKinect()
{
//	int flags = NODE_TYPE_NONE;
//	if (mEnableDepth)
//		flags |= NODE_TYPE_DEPTH;
//	if (mEnableScene)
//		flags |= NODE_TYPE_DEPTH | NODE_TYPE_USER;
	

}

void NodeKinect::run()
{
	// Only one nodekinect can work with the device at once.
	// while this thread is running we attempt to grab a lock
	// on the mutex to allow us to be working with the device.
	
	// the device is only ever initialised once as reinitialising
	// it after releasing it doesn't appear to work with openni
	
	while (!isRequestedToStop())
	{
		mutex::scoped_try_lock tryLock(mDeviceMutex);
		if (tryLock.owns_lock())
		{
			try
			{
				processDevice();
			}
			catch (FailedToOpenKinectException const& e)
			{
				hm_error("Failed to open kinect.");
				break;
			}
			catch (...)
			{
				hm_error("Unhandled exception from OpenNI/Kinect");
			}
		}
		else
		{
			this_thread::sleep_for(chrono::milliseconds(200));
		}
	}
}


void NodeKinect::processDevice()
{
	OpenNIDeviceManager& deviceManager = OpenNIDeviceManager::Instance();
	if (mDevice == nullptr)
	{
		deviceManager.createDevices(1, NODE_TYPE_DEPTH | NODE_TYPE_USER);
		mDevice = deviceManager.getDevice(0);
		if (!mDevice)
		{
			throw FailedToOpenKinectException();
		}
		hm_info("Opened Kinect.");
		mDevice->setDepthShiftMul(3);
	}
		
	while (!isRequestedToStop())
	{
		deviceManager.update();
		assert(mDevice->_isUserOn);
		if (mEnableScene && mDevice->isUserDataNew())
		{
			double timestamp = elapsedTime();
			OpenNIUserList users = deviceManager.getUserList();
			Scene3d scene = Scene3d(timestamp, 0, sceneMeta());
			for (OpenNIUserRef user: users)
			{
				Skeleton3d skeleton(timestamp, user->getId(), sceneMeta());
				skeleton.id = user->getId();
				for (int i=0; i<NUM_JOINTS; i++)
				{
					OpenNIBone const& bone = *user->getBone(jointToVBone(i));
					// OpenNI gives coordinates in a left handed system with
					// x going to the right. We convert these to the MS Kinect
					// SDK standard and use a right handed system with x going
					// to the left. Also OpenNI uses millimetres whereas we
					// use metres (like MS SDK).
					skeleton.joint(i) = Point3d(ci::Vec3f(-bone.position[0], bone.position[1], bone.position[2]) / 1000.f, timestamp, i, sceneMeta());
					skeleton.jointProjective(i) = Point3d(ci::Vec3f(bone.positionProjective), timestamp, i, sceneMeta());
					skeleton.jointConfidence(i) = bone.positionConfidence;
				}
				scene.value.push_back(skeleton);
			}
			Data data(scene);
			mSceneOutlet->outputNewData(data);
		}
		else
		{
			boost::this_thread::sleep_for(boost::chrono::microseconds(250));
		}
	}
}


#endif