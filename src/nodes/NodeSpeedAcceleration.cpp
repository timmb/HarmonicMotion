#include "NodeSpeedAcceleration.h"
#include "Inlet.h"
#include "Outlet.h"

using namespace hm;

NodeSpeedAcceleration::NodeSpeedAcceleration(Node::Params params, std::string className)
: NodeUnthreaded(params, className)
, mLastTimestamp(-42)
{
	createInlet(SCALAR_TYPES | VECTOR2D_TYPES | VECTOR3D_TYPES, "Input", "Values received here will have their speed and acceleration calculated.");
	createOutlet(SCALAR_TYPES, "Speed", "The speed that input values are changing.");
	createOutlet(SCALAR_TYPES, "Acceleration", "The rate at which input values are accelerating.");
}

NodePtr NodeSpeedAcceleration::create(Node::Params params) const
{
	return NodePtr(new NodeSpeedAcceleration(params));
}

void NodeSpeedAcceleration::step()
{
	Data data = inlet(0)->dataIfNewerThan(mLastTimestamp);
	if (!data.isNull())
	{
		double t = data.timestamp();
		double dt = t - mLastTimestamp;
		Data speed = ((data - mLastData) / dt).magnitude();
		Data acceleration = (speed - mLastSpeed) / dt;
		mLastData = data;
		if (speed.isNull())
		{
			// this happens if incompatible types have been received
			// in sequence. need to prevent it permanently making
			// everything null from here on. Set it to zero in that
			// case
			mLastSpeed = data.additiveIdentity();
		}
		else
		{
			mLastSpeed = speed;
			outlet(0)->outputNewData(speed);
		}
		if (!acceleration.isNull())
		{
			outlet(1)->outputNewData(acceleration);
		}
	}
}