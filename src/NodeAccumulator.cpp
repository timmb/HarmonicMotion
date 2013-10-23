#include "NodeAccumulator.h"
#include "Inlet.h"

using namespace hm;

NodeAccumulator::NodeAccumulator(Params const& params, std::string const& className)
: Node(className)
, mInlet(new Inlet(ALL_TYPES, "Data to accumulate", "Data received here is accumulated until it is removed with next() (unless the buffer is full in which case it is ignored"))
, mParams(params)
, mBuffer(params.bufferSize)
{
	addInlet(mInlet);
}

NodeAccumulator::~NodeAccumulator()
{
	stopAndWait();
}

bool NodeAccumulator::isEmpty() const
{
	return !mBuffer.isNotEmpty();
}

bool NodeAccumulator::isFull() const
{
	return !mBuffer.isNotFull();
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