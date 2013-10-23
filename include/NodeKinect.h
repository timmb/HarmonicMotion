#pragma once

#include "Node.h"

namespace V
{
	class OpenNIDevice;
	class OpenNIDeviceManager;
}

namespace hm
{
	class SceneMeta;
	
	class NodeKinect : public Node
	{
	public:
		struct Params
		{
			/// Will force enableDepth
			bool enableScene;
			bool enableDepth;
			
			Params()
			: enableScene(true)
			, enableDepth(true)
			{}
		};
		
		NodeKinect(Params const& params = Params(), std::string const& className="NodeKinect");
		virtual ~NodeKinect();
		
		SceneMetaPtr sceneMeta() const { return mMetadata; }
		
	protected:
		virtual void run() override;
		
	private:
		void openKinect();
		
		Params mParams;
		
		OutletPtr mSceneOutlet;
		
		boost::shared_ptr<V::OpenNIDevice> mDevice;
		V::OpenNIDeviceManager* mOpenNi;
		
		SceneMetaPtr mMetadata;
	};
}