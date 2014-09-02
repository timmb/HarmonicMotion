#pragma once

#include "NodeThreaded.h"

#ifdef HM_OPENNI

namespace V
{
	class OpenNIDevice;
	class OpenNIDeviceManager;
}

namespace hm
{
	class SceneMeta;
	
	class NodeKinect : public NodeThreaded
	{
	public:
		NodeKinect(Params const& params, std::string const& className="NodeKinect");
		virtual ~NodeKinect();
		
		SceneMetaPtr sceneMeta() const { return mMetadata; }
		
	protected:
		virtual void run() override;
		virtual NodePtr create(Node::Params params) const override { return NodePtr(new NodeKinect(params)); }
		
	private:
		class FailedToOpenKinectException : public std::exception
		{
			
		};
		
		void openKinect();
		/// \pre This function must only be called when mDeviceMutex
		/// is locked by the calling thread. If the Kinect opens/is opened
		/// OK then this function will not return until isRequestedToStop()
		/// returns true.
		/// \throw FailedToOpenKinectException
		void processDevice();
		
		OutletPtr mSceneOutlet;
		
		static boost::shared_ptr<V::OpenNIDevice> mDevice;
		/// Only allow one instance of this node to use mDevice at once
		static boost::try_mutex mDeviceMutex;
//		std::atomic<V::OpenNIDeviceManager*> mOpenNi;
		
		SceneMetaPtr mMetadata;
		
		/// Will force mEnableDepth
		bool mEnableScene;
		bool mEnableDepth;
	};
}

#include "FactoryNode.h"
hm_register_node(NodeKinect)

#endif


