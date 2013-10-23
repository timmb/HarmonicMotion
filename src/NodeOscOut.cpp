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
#include <boost/format.hpp>

using namespace hm;
using namespace ci;
using namespace ci::osc;

class JointMessage : public Message
{
public:
	JointMessage(std::string const& prefix, const char* jointName, int userId, float confidence, ci::Vec3f const& position)
	{
		static boost::format addressFormat("/%s/%s");
		static boost::format addressFormatNoPrefix("/%s");
		if (prefix=="")
			address = (addressFormatNoPrefix % jointName).str();
		else
			address = (addressFormat % prefix % jointName).str();
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
, mParams(params)
, mOsc(new Sender)
, mInlet(new Inlet(SKELETON3D | SCENE3D, "Data to be sent", "Messages are formatted '[/<prefix>]/joint <jointname> <userid> <confidence> <x> <y> <z>' using world coordinates."))
, mLastSentTimestamp(-42.)
{
	addInlet(mInlet);
}

NodeOscOut::~NodeOscOut()
{
	stopAndWait();
}

void NodeOscOut::send(Skeleton3d const& skel)
{
	for (int i=0; i<NUM_JOINTS; i++)
	{
		Message m = JointMessage(mParams.prefix, jointName(i), skel.id(), skel.jointConfidence(i), skel.joint(i));
		mOsc->sendMessage(m);
	}
}

void NodeOscOut::send(Scene3d const& scene)
{
	static boost::format addressFormatPrefix("/%s/num_users");
	Message m;
	m.setAddress(mParams.prefix==""? "/num_users" : (addressFormatPrefix % mParams.prefix).str());
	m.addIntArg(scene.skeletons().size());
	mOsc->sendMessage(m);
	for (Skeleton3d const& skel: scene.skeletons())
	{
		send(skel);
	}
}

void NodeOscOut::run()
{
	try
	{
		mOsc->setup(mParams.destinationHost, mParams.destinationPort);
	}
	catch (std::exception& e)
	{
		std::cerr << "Error opening "<<mParams.destinationHost<<":"<<mParams.destinationPort<<" "<<e.what()<<std::endl;
		return;
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
		}
	}
}