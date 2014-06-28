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
#include "Common.h"
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
		
		NodeOscOut(Params params, std::string className="NodeOscOut");
		
	protected:
		virtual NodePtr create(Params params) const override;
		virtual void step() override;

	private:
		/// Visitor for boost::variant
		/// Sends data formatted according to the Harmonic Motion protocol
		class DataSender
		{
		public:
			DataSender(NodeOscOut& nodeOscOut);
			
			typedef void result_type;
			
			void operator()(DataNull const& x);
			/// Not currently supported
			void operator()(Image2d const& x);
			void operator()(Value const& x);
			void operator()(Point2d const& x);
			void operator()(Point3d const& x);
			void operator()(Skeleton3d const& x);
			void operator()(Scene3d const& x);
			void operator()(ListValue const& x);
			void operator()(ListPoint2d const& x);
			void operator()(ListPoint3d const& x);
			
		private:
			NodeOscOut& mNode;
		};
		
		
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
		
		// MARK: Cached values - has a slash at the front and back
		std::string mPrefixWithSlash;
	};
}

#include "FactoryNode.h"
hm_register_node(NodeOscOut);
