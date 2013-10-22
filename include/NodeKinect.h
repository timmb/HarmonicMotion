#pragma once

#include "Node.h"

namespace V
{
	class OpenNIDevice;
	class OpenNIDeviceManager;
}

namespace hm
{
	class NodeKinect : public Node
	{
	public:
		struct Params
		{
			bool enableScene;
			
			Params()
			: enableScene(true)
			{}
		};
		
		NodeKinect(Params const& params = Params(), std::string const& className="NodeKinect");
		
	protected:
		virtual void run() override;
		
	private:
		void openKinect();
		
		Params mParams;
		
		OutletPtr mSceneOutlet;
		
		boost::shared_ptr<V::OpenNIDevice> mDevice;
		V::OpenNIDeviceManager* mOpenNi;
	};
}