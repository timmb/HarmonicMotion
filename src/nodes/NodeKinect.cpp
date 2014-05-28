#include "NodeKinect.h"
#include "VOpenNIHeaders.h"
#include "Outlet.h"
#include "Scene3d.h"
#include <boost/thread/thread.hpp>
#include "SceneMeta.h"

using namespace hm;
using namespace V;

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
: NodeThreaded(params, className)
, mParams(params)
, mSceneOutlet(nullptr)
, mDevice(nullptr)
, mOpenNi(nullptr)
, mMetadata(nullptr)
{
	OpenNIDeviceManager::USE_THREAD = false;
	mOpenNi = OpenNIDeviceManager::InstancePtr();
	mSceneOutlet = createOutlet(SCENE3D, "3D Scene", "All user skeletons in 3D space");
	SceneMeta* metadata = new SceneMeta;
	metadata->cameraPos = ci::Vec3f(0,0,0);
	metadata->cameraDir = ci::Vec3f(0,0,1);
	metadata->cameraFov = 57;
	mMetadata = SceneMetaPtr(metadata);
	
	openKinect();
}

NodeKinect::~NodeKinect()
{
//	std::cout << "~NodeKinect stopping"<<std::endl;;
	stopAndWait();
//	std::cout << "~NodeKinect stopped"<<std::endl;;
}

void NodeKinect::openKinect()
{
	int flags = NODE_TYPE_NONE;
	if (mParams.enableDepth)
		flags |= NODE_TYPE_DEPTH;
	if (mParams.enableScene)
		flags |= NODE_TYPE_DEPTH | NODE_TYPE_USER;
	
	mOpenNi->createDevices(1, NODE_TYPE_DEPTH | NODE_TYPE_USER);
//	mOpenNi->SetPrimaryBuffer(0, NODE_TYPE_USER);
	mDevice = mOpenNi->getDevice(0);
	if (mDevice)
	{
		hm_debug("Opened Kinect.");
		mDevice->setDepthShiftMul(3);
	}
	else
	{
		hm_debug("Failed to open kinect");
	}
}

void NodeKinect::run()
{
	assert(mDevice != nullptr);
	while (!isRequestedToStop())
	{
		mOpenNi->update();
		assert(mDevice->_isUserOn);
		if (mParams.enableScene && mDevice->isUserDataNew())
		{
			double timestamp = elapsedTime();
			OpenNIUserList users = mOpenNi->getUserList();
			Scene3d scene = Scene3d(timestamp, sceneMeta());
			for (OpenNIUserRef user: users)
			{
				Skeleton3d skeleton(timestamp, sceneMeta());
				skeleton.id() = user->getId();
				for (int i=0; i<NUM_JOINTS; i++)
				{
					OpenNIBone const& bone = *user->getBone(jointToVBone(i));
					// OpenNI gives coordinates in a left handed system with
					// x going to the right. We convert these to the MS Kinect
					// SDK standard and use a right handed system with x going
					// to the left. Also OpenNI uses millimetres whereas we
					// use metres (like MS SDK).
					skeleton.joint(i) = Point3d(ci::Vec3f(-bone.position[0], bone.position[1], bone.position[2]) / 1000.f, timestamp, sceneMeta());
					skeleton.jointProjective(i) = Point3d(ci::Vec3f(bone.positionProjective), timestamp, sceneMeta());
					skeleton.jointConfidence(i) = bone.positionConfidence;
				}
				scene.skeletons.push_back(skeleton);
			}
			Data data(scene, elapsedTime());
			mSceneOutlet->outputNewData(data);
		}
		else
		{
			boost::this_thread::sleep_for(boost::chrono::microseconds(250));
		}
	}
	// TODO: Close kinect properly
}


