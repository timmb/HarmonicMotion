#include "NodeMovingAverage.h"
#include "Inlet.h"
#include "Outlet.h"
#include "Data.h"

using namespace hm;

NodeMovingAverage::NodeMovingAverage(Params params, std::string className)
: NodeUnthreaded(params, className)
, mLastTimestamp(-42)
, mDuration(2)
, mBuffer(2)
{
	createInlet(ALL_TYPES, "Input", "Data to be averaged");
	createOutlet(ALL_TYPES, "Output", "Averaged data");
	auto p = addParameter("Duration (s)", &mDuration, "This determines how many seconds into the past the average will be calculated over.");
	p->setBounds(0, mBuffer.MAX_TIME_LIMIT, 0, 100);
}


NodePtr NodeMovingAverage::create(Node::Params params) const
{
	return NodePtr(new NodeMovingAverage(params));
}

void NodeMovingAverage::step()
{
	if (mDuration != mBuffer.timeLimit())
	{
		mBuffer.setTimeLimit(mDuration);
	}
	Data d = inlet(0)->dataIfNewerThan(mLastTimestamp);
	if (!d.isNull())
	{
		// clear the buffer if the type has changed
		if (!mBuffer.empty() && d.type() != mBuffer.back().type())
		{
			mBuffer.clear();
			hm_info("Clearing buffer as input type has changed.");
		}
		if (mBuffer.empty())
		{
			mCurrentAverage = d;
		}
		else
		{
			int n = mBuffer.size();
			mCurrentAverage = mCurrentAverage + (d - mBuffer.front()) * (1. / n);
		}
		// TODO: Also incrementally recalculate to get rid of rounding
		// errors.
		// note, TimeBuffer automatically pops when data gets old
		mBuffer.push_back(d);
		// make a copy as outputNewData modifies its argument
		Data d = mCurrentAverage;
		outlet(0)->outputNewData(mCurrentAverage);
	}
}





