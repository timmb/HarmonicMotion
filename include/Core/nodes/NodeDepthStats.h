#pragma once

#include "NodeUnthreaded.h"

namespace hm
{
	class NodeDepthStats : public NodeUnthreaded
	{
	public:
		NodeDepthStats(Node::Params params, std::string const& className = "NodeDepthStats");

	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;

	private:
		InletPtr mDepthInlet;
		InletPtr mUserIndexInlet;

		OutletPtr mUserPositionOutlet;
		OutletPtr mUserSizeOutlet;

		/// The minimum number of pixels a user must have to be considered a user
		int mMinNumUserPixels;

		double mLastTimestamp;
	};
}
#include "FactoryNode.h"
hm_register_node(NodeDepthStats)