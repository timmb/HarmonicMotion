//
//  GeneratorConstantValue.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "GeneratorSineWave.h"
#include "Outlet.h"
//#include <boost/chrono/duration.hpp>

using namespace hm;

GeneratorSineWave::GeneratorSineWave(Params const& params, std::string const& className)
: Node(className)
, mParams(params)
, mOutlet(new Outlet(VALUE, "Value", "Outputs a value from the sin wave `outputRate` number of times a second"))
, mStartTime(-42)
, mTimeOfLastOutput(-42)
{
	addOutlet(mOutlet);
}

GeneratorSineWave::~GeneratorSineWave()
{
	stopAndWait();
}

void GeneratorSineWave::run()
{
	const double maxSleepDuration(0.2);
	
	
	mStartTime = elapsedTime();
	mTimeOfLastOutput = mStartTime - mParams.outputDelta;
	while (!isRequestedToStop())
	{
		double now = elapsedTime();
		if (now - mTimeOfLastOutput > mParams.outputDelta)
		{
			emitValue(now);
			mTimeOfLastOutput = now;
		}
		double sleepDuration = std::min(maxSleepDuration, now - mTimeOfLastOutput + mParams.outputDelta);
		boost::this_thread::sleep_for(boost::chrono::duration<double, boost::ratio<1>>(sleepDuration));
	}
}

void GeneratorSineWave::emitValue(double now)
{
	double t = now - mStartTime;
	Value value = sinf(float((t * mParams.frequency + mParams.phase) * 2. * M_PI )) * float(mParams.amplitude);
	Data data(value, elapsedTime());
	mOutlet->outputNewData(data);
}

