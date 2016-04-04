#pragma once

#ifdef HM_KINECTSDK

#include "NodeUnthreaded.h"
#include "Kinect2.h"

//namespace Kinect2
//{
//	class Device;
//	class BodyFrame;
//	class DepthFrame;
//	typedef std::shared_ptr<Device> DeviceRef;
//}


namespace hm
{
	class NodeKinect : public NodeUnthreaded
	{
	public:
		NodeKinect(Params const& params, std::string const& className="NodeKinect");
		virtual ~NodeKinect();

		//SceneMetaPtr sceneMeta() const { return mSceneMeta.load(); }

	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void start() override;
		virtual void step() override;
		virtual void stop() override;

	private:
		void callbackBodyFrame(Kinect2::BodyFrame const& frame);
		void callbackDepthFrame(Kinect2::DepthFrame const& frame);
		void callbackBodyIndexFrame(Kinect2::BodyIndexFrame const& frame);
		void callbackInfraredFrame(Kinect2::InfraredFrame const& frame);

		OutletPtr mSceneOutlet;
		OutletPtr mDepthOutlet;
		OutletPtr mBodyIndexOutlet;
		OutletPtr mInfraredOutlet;
		SceneMetaPtr mSceneMeta;
		mutable boost::upgrade_mutex mSceneMetaMutex;

		/// This is equal to nullptr if the Kinect has not been
		/// successfully opened
		Kinect2::DeviceRef mKinect;

		/// Default is to mirror output in the x axis as this is how it arrives from the MS SDK
		/// (mirrored so it matches when you're looking at the sensor and screen simultaneously)
		bool mEnableMirroring;

		/// Because new data arrives through callbacks from
		/// a separate thread we maintain our own variable
		/// to keep track of when that data should be passed
		/// to the outlets
		bool mIsRunning;
		mutable boost::shared_mutex mIsRunningMutex;
		double mTimeOfLastAttemptToOpenKinect;
	};

}

#include "FactoryNode.h"
hm_register_node(NodeKinect)

#endif