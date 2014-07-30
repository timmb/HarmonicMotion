#include "NodePeakDecay.h"
#include "Inlet.h"
#include "Outlet.h"
#include "Functions.h"

using namespace hm;

NodePeakDecay::NodePeakDecay(Node::Params params, std::string className)
: NodeUnthreaded(params, className, "Peak decay", "Calculates the peak value with an adjustable decay.")
, mLastTimestamp(-42)
, mDecayRate(0.9)
{
	createInlet(SCALAR_TYPES, "Input", "Data received here will have it's recent peak value calculated and outputted.");
	createOutlet(SCALAR_TYPES, "Output", "The peak value will be outputted here.");
	
	auto p = addParameter("Decay rate", &mDecayRate, "This determines how slowly the peak falls with time. A value of 1 means it never falls, 0 means it falls isntantly.");
	p->setBounds(0, 1, 0, 1);
}

NodePtr NodePeakDecay::create(Node::Params params) const
{
	return NodePtr(new NodePeakDecay(params));
}

void NodePeakDecay::step()
{
	Data data = inlet(0)->dataIfNewerThan(mLastTimestamp);
	if (!data.isNull())
	{
		// if the datatype has changed then just take the new one
		if (data.type() != mPeak.type())
		{
			mPeak = data;
		}
		else
		{
//			// adjust decay rate based on time between data
//			double dt = data.timestamp() - mLastTimestamp;
//			double adjustedDecayRate = 1. - dt * (1. - mDecayRate);
			double adjustedDecayRate = mDecayRate;
			mPeak = maximum(mPeak * adjustedDecayRate, data);
		}
		Data output = mPeak;
		outlet(0)->outputNewData(output);
		mLastTimestamp = data.timestamp();
	}
}


