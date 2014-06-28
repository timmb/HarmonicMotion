#pragma once

#include "NodeUnthreaded.h"

namespace hm
{
	class NodeSelectJoint : public NodeUnthreaded
	{
	public:
		NodeSelectJoint(Params const& params, std::string const& className="NodeSelectJoint");
		
	protected:
		virtual NodePtr create(Params params) const override;
		virtual void step() override;
		
	private:
		void setupOutlets();
		static int const MAX_NUM_OUTLETS = 15;
		// Up to MAX_NUM_OUTLETS outlets
		int mNumOutlets;
		// These are tracked by parameters so this list is
		// always of length MAX_NUM_OUTLETS
		std::vector<Joint> mSelectedJoints;
		// Parameters that control mSelectedJoints
		std::vector<ParameterPtr> mSelectedJointParameters;
		double mLastTimestamp;
	};
}
#include "FactoryNode.h"
hm_register_node(NodeSelectJoint, "Select joint", "Choose a joint from a skeleton.")
