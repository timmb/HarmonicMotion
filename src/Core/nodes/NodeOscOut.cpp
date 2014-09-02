//
//  NodeOscOut.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "nodes/NodeOscOut.h"
#include "OscSender.h"
#include "Inlet.h"
#include <boost/thread/locks.hpp>
#include "FactoryNode.h"
#include "Point2d.h"

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
: NodeUnthreaded(params, className, "OSC out", "Sends received data to another application via Open Sound Control (OSC)")
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
	p->setBounds(0, 65535, 0, 65535);
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


NodeOscOut::DataSender::DataSender(NodeOscOut& node)
: mNode(node)
{}


void NodeOscOut::DataSender::operator()(DataNull const& x)
{
	
}


void NodeOscOut::DataSender::operator()(Image2d const& x)
{
	
}



void NodeOscOut::DataSender::operator()(Value const& value)
{
	Message m;
	m.setAddress(mNode.mPrefixWithSlash);
	m.addFloatArg(value.value);
	mNode.mOsc.sendMessage(m);
}


void NodeOscOut::DataSender::operator()(Point2d const& value)
{
	Message m;
	m.setAddress(mNode.mPrefixWithSlash);
	m.addFloatArg(value.value.x);
	m.addFloatArg(value.value.y);
	mNode.mOsc.sendMessage(m);
}


void NodeOscOut::DataSender::operator()(Point3d const& value)
{
	Message m;
	m.setAddress(mNode.mPrefixWithSlash);
	m.addFloatArg(value.value.x);
	m.addFloatArg(value.value.y);
	m.addFloatArg(value.value.z);
	mNode.mOsc.sendMessage(m);
}

void NodeOscOut::DataSender::operator()(Skeleton3d const& skel)
{
	for (int i=0; i<NUM_JOINTS; i++)
	{
		JointMessage m(mNode.mPrefixWithSlash, jointName(i), skel.id, skel.jointConfidence(i), skel.joint(i).value);
		mNode.mOsc.sendMessage(m);
	}
}

void NodeOscOut::DataSender::operator()(Scene3d const& scene)
{
	Message m;
	m.setAddress(mNode.mPrefixWithSlash+"num_users");
	m.addIntArg(scene.value.size());
	mNode.mOsc.sendMessage(m);
	for (Skeleton3d const& skel: scene.value)
	{
		(*this)(skel);
	}
}

void NodeOscOut::DataSender::operator()(ListValue const& x)
{
	Message m;
	m.setAddress(mNode.mPrefixWithSlash);
	for (Value const& y: x.value)
	{
		m.addFloatArg(y.value);
	}
	mNode.mOsc.sendMessage(m);
}

void NodeOscOut::DataSender::operator()(ListPoint2d const& x)
{
	Message m;
	m.setAddress(mNode.mPrefixWithSlash);
	for (Point2d const& y: x.value)
	{
		m.addFloatArg(y.value.x);
		m.addFloatArg(y.value.y);
	}
	mNode.mOsc.sendMessage(m);
}

void NodeOscOut::DataSender::operator()(ListPoint3d const& x)
{
	Message m;
	m.setAddress(mNode.mPrefixWithSlash);
	for (Point3d const& y: x.value)
	{
		m.addFloatArg(y.value.x);
		m.addFloatArg(y.value.y);
		m.addFloatArg(y.value.z);
	}
	mNode.mOsc.sendMessage(m);
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
			std::stringstream ss;
			ss << "Error opening " << mDestinationHost << ":" << mDestinationPort << " " << e.what();
			hm_error(ss.str());
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
			DataSender sender(*this);
			boost::apply_visitor(sender, data.data);
			mLastSentTimestamp = data.timestamp();
			std::cout << "Sent data with timestamp "<<mLastSentTimestamp << std::endl;
		}
	}
}



