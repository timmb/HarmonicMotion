#include "nodes/NodeCompareToHistory.h"
#include "Inlet.h"
#include "Outlet.h"
#include "Parameter.h"

using namespace hm;

NodeCompareToHistory::NodeCompareToHistory(Params const& params, std::string const& className)
: NodeUnthreaded(params, className, "Compare to history", "Calculates the difference/min/max between the current value and the past N values.")
, mLastTimestamp(-42)
{
	mInlet = createInlet(ALL_TYPES, "Input", "A comparison will be made between current data received here and past data received here.");
	mOutlet = createOutlet(ALL_TYPES, "Output", "The comparison result is outputted here.");
	addParameter("Window size", &mMovingAverage.mNumSamples, "How many frames into the past to average over when making the comparison.")->setBounds(0, 999, 0, 50);
	
	std::vector<std::string> comparisonLabels = {
		"Difference",
		"Maximum",
		"Minimum"
	};
	auto p = addParameter("Comparison", &mComparison, "What comparison technique to compare current values against past values");
	p->setEnumerationLabels(comparisonLabels);
	p->addNewExternalValueCallback([&]() {
		comparisonChangedCallback();
	});
	
}

NodePtr NodeCompareToHistory::create(Params params) const
{
	return NodePtr(new NodeCompareToHistory(params));
}

void NodeCompareToHistory::step()
{
	if (mInlet->dataTimestamp() > mLastTimestamp)
	{
		Data now = mInlet->data();
		Data past = mMovingAverage.update(now);
		Data result;
		if (!now.isNull() && !past.isNull())
		{
			switch (mComparison)
			{
			case DIFF:
				result = now - past;
				break;
			case MAXIMUM:
				result = maximum(now, past);
				break;
			case MINIMUM:
				result = minimum(now, past);
				break;
			default:
				assert(false);
			}
		}
		if (!result.isNull())
		{
			mOutlet->outputNewData(result);
		}
		mLastTimestamp = now.timestamp();
	}
}

void NodeCompareToHistory::comparisonChangedCallback()
{
}