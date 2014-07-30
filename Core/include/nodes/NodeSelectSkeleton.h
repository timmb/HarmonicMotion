#pragma once
#include "NodeUnthreaded.h"

namespace hm
{
	class NodeSelectSkeleton : public NodeUnthreaded
	{
	public:
		NodeSelectSkeleton(Node::Params params, std::string className="NodeSelectSkeleton");
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		enum Mode
		{
			CLOSEST,
			NUM_MODES
		};
		
		int mMode;
		double mLastDataTimestamp;
	};
}
#include "FactoryNode.h"
hm_register_node(NodeSelectSkeleton)
