#pragma once

#include "NodeThreaded.h"

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
		struct Params : public Node::Params
		{
			/// Will force enableDepth
			bool enableScene;
			bool enableDepth;
			
			Params(bool enableScene_=true, bool enableDepth_=true, std::string const& name_="")
			: Node::Params(name_)
			, enableScene(true)
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