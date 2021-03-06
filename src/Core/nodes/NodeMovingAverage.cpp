#include "nodes/NodeMovingAverage.h"
#include "Inlet.h"
#include "Outlet.h"
#include "Data.h"

using namespace hm;

MovingAverage::MovingAverage()
: mNumSamples(40)
, mBuffer(40)
{}

Data MovingAverage::update(Data const& d)
{
	if (mNumSamples != mBuffer.capacity())
	{
		mBuffer.set_capacity(mNumSamples);
	}

	// clear the buffer if the type has changed
	if (!mBuffer.empty() && (d.type() != mBuffer.back().type() || !d.canAdd(mBuffer.back())))
	{
		mBuffer.clear();
		hm_info("Clearing buffer as input type has changed.");
	}
	if (mBuffer.empty())
	{
		mCurrentSum = d;
	}
	else
	{
		// do addition this way as the left hand operand keeps
		// any extra list elements if they are not present in the
		// right hand side. this allows e.g. skeletons to come and
		// leave a scene.
		mCurrentSum = d + mCurrentSum;
		if (mBuffer.full())
		{
			mCurrentSum = mCurrentSum - mBuffer.front();
			mBuffer.pop_front();
		}
	}
	// TODO: Also incrementally recalculate to get rid of rounding
	// errors.
	mBuffer.push_back(d);
	// make a copy as outputNewData modifies its argument
	Data ret = mCurrentSum / mBuffer.size();
	assert(ret.type() == d.type());
	return ret;
}

NodeMovingAverage::NodeMovingAverage(Params params, std::string className)
: NodeUnthreaded(params, className, "Moving average", "Finds the average value over the past n samples")
, mLastTimestamp(-42)
{
	createInlet(ALL_TYPES, "Input", "Data to be averaged");
	createOutlet(ALL_TYPES, "Output", "Averaged data");
	auto p = addParameter("Number of samples", &mMovingAverage.mNumSamples, "This determines over how far into the past the average will be calculated.");
	p->setBounds(0, 100000, 0, 1000);
}


NodePtr NodeMovingAverage::create(Node::Params params) const
{
	return NodePtr(new NodeMovingAverage(params));
}

void NodeMovingAverage::step()
{
	Data d = inlet(0)->dataIfNewerThan(mLastTimestamp);
	if (!d.isNull())
	{
        Data data = mMovingAverage.update(d);
		outlet(0)->outputNewData(data);
	}
}





