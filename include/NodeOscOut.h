//
//  NodeOscOut.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once
#include "Node.h"

namespace cinder
{
	namespace osc
	{
		class Sender;
	}
}

namespace hm
{
	class NodeOscOut : public Node
	{
	public:
		struct Params
		{
			std::string destinationHost;
			int destinationPort;
			/// Messages are formatted as /prefix/jointName
			std::string prefix;
			
			Params()
			: destinationHost("localhost")
			, destinationPort(20000)
			, prefix("skel")
			{}
		};
		
		NodeOscOut(Params const& params = Params(), std::string const& className="NodeOscOut");
		virtual ~NodeOscOut();
		
	protected:
		virtual void run() override;

	private:
		void send(Skeleton3d const& skeleton);
		void send(Scene3d const& scene);
		
		Params mParams;
		std::unique_ptr<ci::osc::Sender> mOsc;
		InletPtr mInlet;
		double mLastSentTimestamp;
	};
}