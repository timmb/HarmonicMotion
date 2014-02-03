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

using namespace hm;
using namespace ci;
using namespace ci::osc;
using namespace boost;

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

NodeOscOut::NodeOscOut(Params const& params, std::string const& className)
: NodeUnthreaded(params, className)
//, mOsc(new Sender)
, mInlet(nullptr)
, mLastSentTimestamp(-42.)
{
	setParams(params);
	mInlet = createInlet(VALUE | SKELETON3D | SCENE3D,
						 "Data to be sent",
						 "Messages are formatted '[/<prefix>]/joint <jointname> <userid> <confidence> <x> <y> <z>' using world coordinates.");
}

void NodeOscOut::setParams(Params params)
{
	unique_lock<shared_mutex> lock(mParamsMutex);
	mParams = params;
	if (!mParams.prefix.empty() && mParams.prefix[0] != '/')
	{
		mPrefixWithSlash = '/'+mParams.prefix;
	}
	if (mPrefixWithSlash.size()>1 && mPrefixWithSlash[mPrefixWithSlash.size()-1] == '/')
	{
		mPrefixWithSlash = mPrefixWithSlash.substr(0, mPrefixWithSlash.size()-1);
	}
	setNodeParams(params);
}

NodeOscOut::Params NodeOscOut::params() const
{
	shared_lock<shared_mutex> lock(mParamsMutex);
	return mParams;
}


void NodeOscOut::send(Value const& value)
{
	Message m;
	{
		shared_lock<shared_mutex> lock(mParamsMutex);
		m.setAddress(mPrefixWithSlash);
	}
	m.addFloatArg(value.value());
	//	m.setRemoteEndpoint(mParams.destinationHost, mParams.destinationPort);
	mOsc.sendMessage(m);
}

void NodeOscOut::send(Skeleton3d const& skel)
{
	for (int i=0; i<NUM_JOINTS; i++)
	{
		shared_lock<shared_mutex> lock(mParamsMutex);
		Message m = JointMessage(mPrefixWithSlash, jointName(i), skel.id(), skel.jointConfidence(i), skel.joint(i));
		mOsc.sendMessage(m);
	}
}

void NodeOscOut::send(Scene3d const& scene)
{
	Message m;
	{
		shared_lock<shared_mutex> lock(mParamsMutex);
		m.setAddress(mPrefixWithSlash+"num_users");
	}
	m.addIntArg(scene.skeletons().size());
	mOsc.sendMessage(m);
	for (Skeleton3d const& skel: scene.skeletons())
	{
		send(skel);
	}
}

void NodeOscOut::step()
{
	// TODO: Change to use callback and Parameter
	{
		shared_lock<shared_mutex> lock(mParamsMutex);
		try
		{
			mOsc.setup(mParams.destinationHost, mParams.destinationPort);
		}
		catch (std::exception& e)
		{
			std::cerr << "Error opening "<<mParams.destinationHost<<":"<<mParams.destinationPort<<" "<<e.what()<<std::endl;
			return;
		}
	}
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