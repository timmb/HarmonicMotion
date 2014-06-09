//
//  NodeSineWave.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "NodeSineWave.h"
#include "Outlet.h"
#include "FactoryNode.h"
#include "Common.h"

using namespace hm;

NodeSineWave::NodeSineWave(Node::Params params, std::string className)
: NodeThreaded(params, className)
, mOutlet(nullptr)
, mStartTime(-42)
, mTimeOfLastOutput(-42)
, mFrequency(0.2)
, mPhase(0.)
, mAmplitude(1.)
, mOutputDelta(0.3)
{
	mOutlet = createOutlet(VALUE, "Value", "Outputs a value from the sin wave `outputRate` number of times a second");
	// TODO: temp
	createOutlet(VALUE, "Test outlet", "temp test outlet");
	createOutlet(VALUE, "Test outlet 2", "temp test outlet");
	addParameter("Frequency", &mFrequency)->setBounds(0, 999999, 0, 999999);
	addParameter("Phase (0-1)", &mPhase)->setBounds(0, 1, 0, 1);
	addParameter("Amplitude", &mAmplitude)->setBounds(-999999, 999999, -9, +9);
	addParameter("Time between outputs (s)", &mOutputDelta)->setBounds(0, 999999, 0, 999);
}

NodeSineWave::~NodeSineWave()
{
	stopAndWait();
}


NodePtr NodeSineWave::create(Node::Params params) const
{
	return NodePtr(new NodeSineWave(params));
}

void NodeSineWave::run()
{
	const double maxSleepDuration(0.2);
	
	
	mStartTime = elapsedTime();
	mTimeOfLastOutput = mStartTime - mOutputDelta;
	while (!isRequestedToStop())
	{
		double now = elapsedTime();
		if (now - mTimeOfLastOutput > mOutputDelta)
		{
			emitValue(now);
			mTimeOfLastOutput = now;
		}
		double sleepDuration = std::min(maxSleepDuration, now - mTimeOfLastOutput + mOutputDelta);
		boost::this_thread::sleep_for(boost::chrono::duration<double, boost::ratio<1>>(sleepDuration));
	}
}

void NodeSineWave::emitValue(double now)
{
	double t = now - mStartTime;
	Value value(sinf(float((t * mFrequency + mPhase) * 2. * M_PI )) * float(mAmplitude), elapsedTime());
	Data data(value);
	mOutlet->outputNewData(data);
}

