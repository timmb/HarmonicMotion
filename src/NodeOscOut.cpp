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
: Node(className)
//, mOsc(new Sender)
, mInlet(new Inlet(VALUE | SKELETON3D | SCENE3D,
				   "Data to be sent",
				   "Messages are formatted '[/<prefix>]/joint <jointname> <userid> <confidence> <x> <y> <z>' using world coordinates."))
, mLastSentTimestamp(-42.)
{
	setParams(params);
	addInlet(mInlet);
	// Ensure prefix starts but does not end with slash
}

NodeOscOut::~NodeOscOut()
{
	stopAndWait();
}

void NodeOscOut::setParams(Params const& params)
{
	unique_lock<shared_mutex> lock(mMutex);
	mParams = params;
	if (!mParams.prefix.empty() && mParams.prefix[0] != '/')
	{
		mPrefixWithSlash = '/'+mParams.prefix;
	}
	if (mPrefixWithSlash.size()>1 && mPrefixWithSlash[mPrefixWithSlash.size()-1] == '/')
	{
		mPrefixWithSlash = mPrefixWithSlash.substr(0, mPrefixWithSlash.size()-1);
	}
}

NodeOscOut::Params NodeOscOut::params() const
{
	shared_lock<shared_mutex> lock(mMutex);
	return mParams;
}

void NodeOscOut::send(Value const& value)
{
	Message m;
	{
		shared_lock<shared_mutex> lock(mMutex);
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
		shared_lock<shared_mutex> lock(mMutex);
		Message m = JointMessage(mPrefixWithSlash, jointName(i), skel.id(), skel.jointConfidence(i), skel.joint(i));
		mOsc.sendMessage(m);
	}
}

void NodeOscOut::send(Scene3d const& scene)
{
	Message m;
	{
		shared_lock<shared_mutex> lock(mMutex);
		m.setAddress(mPrefixWithSlash+"num_users");
	}
	m.addIntArg(scene.skeletons().size());
	mOsc.sendMessage(m);
	for (Skeleton3d const& skel: scene.skeletons())
	{
		send(skel);
	}
}

void NodeOscOut::run()
{
	{
		shared_lock<shared_mutex> lock(mMutex);
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
	while (!isRequestedToStop())
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