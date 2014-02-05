//
//  NodeOscOut.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "NodeOscOut.h"
#include "OscSender.h"
#include "Inlet.h"
#include <boost/thread/locks.hpp>
#include "FactoryNode.h"

using namespace hm;
using namespace ci;
using namespace ci::osc;
using namespace boost;
using std::string;

class JointMessage : public Message
{
public:
	JointMessage(std::string const& prefix, const char* jointName, int userId, float confidence, ci::Vec3f const& position)
	{
		assert(!prefix.empty() && prefix[0]=='/');
		address = prefix + '/' + jointName;
		args = {
			new ArgInt32(userId),
			new ArgFloat(confidence),
			new ArgFloat(position.x),
			new ArgFloat(position.y),
			new ArgFloat(position.z),
		};
	}
};

NodeOscOut::NodeOscOut(Params params, std::string className)
: NodeUnthreaded(params, className)
//, mOsc(new Sender)
, mInlet(nullptr)
, mLastSentTimestamp(-42.)
, mDestinationHost("localhost")
, mDestinationPort(7110) // OSCeleton default
, mPrefix("hm")
, mDestinationHasChanged(true)
, mIsSocketOpen(false)
{
	mInlet = createInlet(VALUE | SKELETON3D | SCENE3D,
						 "Data to be sent",
						 "Messages are formatted '[/<prefix>]/joint <jointname> <userid> <confidence> <x> <y> <z>' using world coordinates.");
	addParameter("Destination hostname", &mDestinationHost)->addNewExternalValueCallback([=](){ callbackDestinationChanged(); });
	auto p = addParameter("Destination port", &mDestinationPort);
	p->hardMin = p->softMin = 0;
	p->hardMax = p->softMax = 65535;
	p->addNewExternalValueCallback([=](){ callbackDestinationChanged(); });
	addParameter("OSC address prefix", &mPrefix)->addNewExternalValueCallback([=](){ callbackOscAddressChanged(); });
}

NodePtr NodeOscOut::create(Params params) const
{
	return NodePtr(new NodeOscOut(params));
}


void NodeOscOut::callbackOscAddressChanged()
{
	if (!mPrefix.empty() && mPrefix[0] != '/')
	{
		mPrefixWithSlash = '/'+mPrefix;
	}
	if (mPrefixWithSlash.size()>1 && mPrefixWithSlash[mPrefixWithSlash.size()-1] == '/')
	{
		mPrefixWithSlash = mPrefixWithSlash.substr(0, mPrefixWithSlash.size()-1);
	}
}

void NodeOscOut::callbackDestinationChanged()
{
	mDestinationHasChanged = true;
}


void NodeOscOut::send(Value const& value)
{
	Message m;
	m.setAddress(mPrefixWithSlash);
	m.addFloatArg(value.value());
	//	m.setRemoteEndpoint(mParams.destinationHost, mParams.destinationPort);
	mOsc.sendMessage(m);
}

void NodeOscOut::send(Skeleton3d const& skel)
{
	for (int i=0; i<NUM_JOINTS; i++)
	{
		Message m = JointMessage(mPrefixWithSlash, jointName(i), skel.id(), skel.jointConfidence(i), skel.joint(i));
		mOsc.sendMessage(m);
	}
}

void NodeOscOut::send(Scene3d const& scene)
{
	Message m;
	m.setAddress(mPrefixWithSlash+"num_users");
	m.addIntArg(scene.skeletons().size());
	mOsc.sendMessage(m);
	for (Skeleton3d const& skel: scene.skeletons())
	{
		send(skel);
	}
}

void NodeOscOut::step()
{
	if (mDestinationHasChanged)
	{
		try
		{
			mOsc.setup(mDestinationHost, mDestinationPort);
			mIsSocketOpen = true;
		}
		catch (std::exception& e)
		{
			hm_error((std::stringstream()<<"Error opening "<<mDestinationHost<<":"<<mDestinationPort<<" "<<e.what()).str());
			mIsSocketOpen = false;
			return;
		}
		mDestinationHasChanged = false;
	}
	if (mIsSocketOpen)
	{
		Data data = mInlet->dataIfNewerThan(mLastSentTimestamp);
		if (!data.isNull())
		{
			if (data.isSkeleton3d())
			{
				send(data.asSkeleton3d());
			}
			else if (data.isScene3d())
			{
				send(data.asScene3d());
			}
			else if (data.isValue())
			{
				send(data.asValue());
			}
		}
	}
}