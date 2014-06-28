#pragma once

#include "NodeUnthreaded.h"
#include "opencv2/features2d/features2d.hpp"

namespace hm
{
	/// Tracks blobs from a 2D image
	class NodeBlobTracker : public NodeUnthreaded
	{
	public:
		NodeBlobTracker(Node::Params const& params, std::string const& className="NodeBlobTracker");
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		// Helper function for setting up paramters
		void addFilterParams(bool& filterOnOff, float& filterMin, float& filterMax, char const* name, double softMin, double softMax);
		
		InletPtr mInlet;
		OutletPtr mBlobOutlet;
		OutletPtr mSizeOutlet;
		double mTimestampOfLastData;
		bool mForceUpdate;
		
		cv::SimpleBlobDetector::Params mBlobParams;
	};
}
#include "FactoryNode.h"
hm_register_node(NodeBlobTracker)

