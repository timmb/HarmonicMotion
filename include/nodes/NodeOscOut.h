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
		
	protected:
		virtual NodePtr create(Params params) override;
		virtual void step() override;

	private:
		void send(Value const& value);
		void send(Skeleton3d const& skeleton);
		void send(Scene3d const& scene);
		
		void callbackOscAddressChanged();
		void callbackDestinationChanged();

		ci::osc::Sender mOsc;
		InletPtr mInlet;
		
		// MARK: State
		double mLastSentTimestamp;
		bool mDestinationHasChanged;
		bool mIsSocketOpen;
		
		// MARK: Parameters
		// These are thread-safe as Parameters are always updated outside of
		// the step() function
		std::string mDestinationHost;
		int mDestinationPort;
		/// Prefix used to form OSC addresses
		std::string mPrefix;
		
		// MARK: Cached values
		std::string mPrefixWithSlash;
	};
}