#include "nodes/NodeAccumulator.h"
#include "Inlet.h"

using namespace hm;

NodeAccumulator::NodeAccumulator(Params const& params, std::string const& className)
: NodeThreaded(params, className, "Accumulator", "Accumulates data until it is removed with next() or the buffer fills")
, mInlet(nullptr)
, mParams(params)
, mBuffer(params.bufferSize)
{
	mInlet = createInlet(ALL_TYPES, "Data to accumulate", "Data received here is accumulated until it is removed with next() (unless the buffer is full in which case it is ignored");
}

NodeAccumulator::~NodeAccumulator()
{
	stopAndWait();
}

bool NodeAccumulator::isEmpty() const
{
	return !const_cast<ci::ConcurrentCircularBuffer<Data>&>(mBuffer).isNotEmpty();
}

bool NodeAccumulator::isFull() const
{
	return !const_cast<ci::ConcurrentCircularBuffer<Data>&>(mBuffer).isNotFull();
}

Data NodeAccumulator::next()
{
	Data data;
	bool success = mBuffer.tryPopBack(&data);
	assert(success);
	return data;
}

void NodeAccumulator::run()
{
	double lastTimestamp = -42.;
	while (!isRequestedToStop())
	{
		Data data = mInlet->dataIfNewerThan(lastTimestamp);
		if (!data.isNull())
		{
			bool success = mBuffer.tryPushFront(mInlet->data());
			if (!success)
			{
				hm_debug("Data dropped as buffer is full.");
			}
			lastTimestamp = data.timestamp();
		}
		waitForNewData();
	}
}