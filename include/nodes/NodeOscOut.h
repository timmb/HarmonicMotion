//
//  NodeOscOut.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once
#include "NodeUnthreaded.h"
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
	class NodeOscOut : public NodeUnthreaded
	{
	public:
		typedef std::shared_ptr<NodeOscOut> Ptr;
		
		NodeOscOut(Node::Params const& params=Node::Params(), std::string const& className="NodeOscOut");
		
		/// Thread-safe
		void setParams(Params params);
		/// Thread-safe
		Params params() const;

	protected:
		virtual void step() override;

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
		
		// MARK: Parameters
		std::string mDestinationHost;
		int mDestinationPort;
		/// Prefix used to form OSC addresses
		std::string mPrefix;
	};
}