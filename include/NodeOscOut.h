//
//  NodeOscOut.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once
#include "Node.h"
#include "OscSender.h"
#include <boost/thread/shared_mutex.hpp>

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
		typedef std::shared_ptr<NodeOscOut> Ptr;
		struct Params : public Node::Params
		{
			std::string destinationHost;
			int destinationPort;
			/// Messages are formatted as /prefix/jointName
			std::string prefix;
			
			Params(std::string const& destinationHost_="localhost", int port_=20000, std::string const& prefix_="hm", std::string const& name_="")
			: Node::Params(name_)
			, destinationHost("localhost")
			, destinationPort(20000)
			, prefix("hm")
			{}
		};
		
		NodeOscOut(Params const& params = Params(), std::string const& className="NodeOscOut");
		virtual ~NodeOscOut();
		
		/// Thread-safe
		void setParams(Params const& params);
		/// Thread-safe
		Params params() const;

	protected:
		virtual void run() override;

	private:
		void send(Value const& value);
		void send(Skeleton3d const& skeleton);
		void send(Scene3d const& scene);

		/// Guards mParams and mPrefixWithSlash
		mutable boost::shared_mutex mParamsMutex;
		Params mParams;
		std::string mPrefixWithSlash;
		ci::osc::Sender mOsc;
		InletPtr mInlet;
		double mLastSentTimestamp;
	};
}