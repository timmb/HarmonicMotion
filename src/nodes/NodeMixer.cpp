#include "NodeMixer.h"
#include "Inlet.h"
#include "Outlet.h"

using namespace hm;

NodeMixer::NodeMixer(Node::Params params, std::string className)
: NodeUnthreaded(params, className, "Mixer", "Mixes between two sources of data")
, mBlendFactor(0.5)
, mLastTimestamp0(-42)
, mLastTimestamp1(-42)
{
	createInlet(ALL_TYPES, "First value", "The first of two values that will be averaged together according to the blend factor.");
	createInlet(ALL_TYPES, "Second value", "The second of two values that will be averaged together according to blend factor.");
	createOutlet(ALL_TYPES, "Output", "The two blended values will be outputted here");
	auto p = addParameter("Blend factor", &mBlendFactor, "How the two values will be blended. 0 means take only the first value. 0.5 means blend them equally. 1 means take only the second value.");
	p->setBounds(0, 1, 0, 1);
}


NodePtr NodeMixer::create(Node::Params params) const
{
	return NodePtr(new NodeMixer(params));
}


void NodeMixer::step()
{
	if (inlet(0)->dataTimestamp() > mLastTimestamp0	&& inlet(1)->dataTimestamp() > mLastTimestamp1)
	{
		Data d0 = inlet(0)->data();
		Data d1 = inlet(1)->data();
		if (d0.canAdd(d1))
		{
			Data output = d0 * (1.-mBlendFactor) + d1 * mBlendFactor;
			outlet(0)->outputNewData(output);
		}
		mLastTimestamp0 = d0.timestamp();
		mLastTimestamp1 = d1.timestamp();
	}
}