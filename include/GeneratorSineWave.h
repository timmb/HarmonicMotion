//
//  GeneratorConstantValue.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once

#include "Node.h"
//#include <boost/chrono.hpp>

namespace hm
{
	/// Testing node to create a sine wave
	class GeneratorSineWave : public Node
	{
	public:
		struct Params : public Node::Params
		{
			/// Cycles per second
			double frequency;
			/// Ranges from 0 to 1
			double phase;
			double amplitude;
			/// How long to wait between outputting values. This class isn't
			/// super-accurate and the actual output rate may differ from
			/// this
			double outputDelta;
			
			Params(double frequency_=0.2, double phase_=0., double amplitude_=1., std::string const& name_="")
			: Node::Params(name_)
			, frequency(frequency_)
			, phase(phase_)
			, amplitude(amplitude_)
			, outputDelta(0.05)
			{}
		};
		
		GeneratorSineWave(Params const& params = Params(), std::string const& className="GeneratorSineWave");
		virtual ~GeneratorSineWave();
		
	protected:
		virtual void run() override;
		
	private:
//		typedef boost::chrono::duration<double> Duration;
//		typedef boost::chrono::time_point<boost::chrono::high_resolution_clock, Duration> Time;

		void emitValue(double currentTime);
		
		Params mParams;
		OutletPtr mOutlet;
		double mStartTime;
		double mTimeOfLastOutput;
//		Time mTimeOfLastOutput;
//		Time mStartTime;
	};
}