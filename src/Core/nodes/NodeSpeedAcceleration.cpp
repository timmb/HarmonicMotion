#include "nodes/NodeSpeedAcceleration.h"
#include "Inlet.h"
#include "Outlet.h"

using namespace hm;

NodeSpeedAcceleration::NodeSpeedAcceleration(Node::Params params, std::string className)
: NodeUnthreaded(params, className, "Speed and Acceleration", "Calculate the speed, velocity and acceleration of values.")
, mLastTimestamp(-42)
{
	createInlet(SCALAR_TYPES | VECTOR2D_TYPES | VECTOR3D_TYPES, "Input", "Values received here will have their speed and acceleration calculated.");
	createOutlet(SCALAR_TYPES, "Speed", "The speed that input values are changing. This is always a positive number.");
	createOutlet(SCALAR_TYPES | VECTOR2D_TYPES | VECTOR3D_TYPES, "Velocity", "The velocity that input values are changing.");
	createOutlet(SCALAR_TYPES, "Scalar acceleration", "The rate at which input values are accelerating, as a scalar value (i.e. the magnitude of 2D/3D vectors.");
	createOutlet(SCALAR_TYPES | VECTOR2D_TYPES | VECTOR3D_TYPES, "Acceleration", "The rate at which input values are acceleration as either a scalar/2D/3D vector based on the input type.");
	
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
		Data velocity = (data - mLastData) / dt;
		Data speed = velocity.magnitude();
		Data vectorAcceleration = (velocity - mLastVelocity) / dt;
		Data scalarAcceleration = (speed - mLastSpeed) / dt;
		mLastData = data;
		if (speed.isNull() || velocity.isNull())
		{
			// this happens if incompatible types have been received
			// in sequence. need to prevent it permanently making
			// everything null from here on. Set it to zero in that
			// case
			mLastSpeed = data.additiveIdentity();
			mLastVelocity = data.additiveIdentity();
		}
		else
		{
			mLastSpeed = speed;
			mLastVelocity = velocity;
			outlet(0)->outputNewData(speed);
			outlet(1)->outputNewData(velocity);
		}
		if (!scalarAcceleration.isNull())
		{
			outlet(2)->outputNewData(scalarAcceleration);
		}
		if (!vectorAcceleration.isNull())
		{
			outlet(3)->outputNewData(vectorAcceleration);
		}
	}
}



