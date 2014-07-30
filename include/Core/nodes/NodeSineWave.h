//
//  NodeSineWave.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once

#include "NodeThreaded.h"
//#include <boost/chrono.hpp>

namespace hm
{
	/// Testing node to create a sine wave
	class NodeSineWave : public NodeThreaded
	{
	public:
		NodeSineWave(Node::Params params, std::string className="NodeSineWave");
		virtual ~NodeSineWave();
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void run() override;
		
	private:
		void emitValue(double currentTime);
		
		OutletPtr mOutlet;
		
		//MARK: State
		float mLastPhase;
		double mTimeOfLastOutput;
		
		//MARK: Parameters
		/// Cycles per second
		double mFrequency;
		/// Initial phase - ranges from 0 to 1
		double mInitialPhase;
		double mAmplitude;
		/// How long to wait between outputting values. This class isn't
		/// super-accurate and the actual output rate may differ from
		/// this
		double mOutputDelta;
	};	
}
#include "FactoryNode.h"
hm_register_node(NodeSineWave)

