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
		NodeKinect(Params const& params, std::string const& className="NodeKinect");
		virtual ~NodeKinect();
		
		SceneMetaPtr sceneMeta() const { return mMetadata; }
		
	protected:
		virtual void run() override;
		virtual NodePtr create(Node::Params params) const override { return NodePtr(new NodeKinect(params)); }
		
	private:
		void openKinect();
		
		OutletPtr mSceneOutlet;
		
		boost::shared_ptr<V::OpenNIDevice> mDevice;
		V::OpenNIDeviceManager* mOpenNi;
		
		SceneMetaPtr mMetadata;
		
		/// Will force mEnableDepth
		bool mEnableScene;
		bool mEnableDepth;
	};
}

#include "FactoryNode.h"
hm_register_node(NodeKinect, "NodeKinect", "Receive skeleton data from a Kinect.")



