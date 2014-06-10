#pragma once

#include "NodeUnthreaded.h"
#include "cinder/Capture.h"
#include "Common.h"

namespace hm
{
	class NodeCamera : public NodeUnthreaded
	{
	public:
		NodeCamera(Node::Params const& params, std::string const& className="NodeCamera");
		~NodeCamera();
		
	protected:
		virtual NodePtr create(Node::Params param) const override;
		
		virtual void start() override;
		virtual void step() override;
		virtual void stop() override;
		
	private:
		void updateCurrentDevice();
		
		// Device 0 is no device
		// Devices 1+ map to the devices reported by the operating system
		
		int mCurrentDevice;
		int mRequestedDevice;
		ParameterPtrT<int> mRequestedDeviceParameter;
		
		int mRequestedWidth;
		int mActualWidth;
		int mRequestedHeight;
		int mActualHeight;
		
		ci::CaptureRef mCapture;
		OutletPtr mOutlet;
	};
}


#include "FactoryNode.h"
hm_register_node(NodeCamera, "Camera", "Receive video from an attached camera.")
