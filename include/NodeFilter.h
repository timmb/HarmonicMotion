//
//  NodeFilter.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 24/10/2013.
//
//

#pragma once

#include "Node.h"
#include "Filter.h"
#include <boost/thread/shared_mutex.hpp>

namespace hm
{
	class NodeFilter : public Node
	{
	public:
		struct Params
		{
			/// First order smoothing on the Predictor
			float alpha;
			/// Second order smoothing on the Predictor
			float beta;
			Params(float alpha_=0.5f, float beta_=0.5f)
			: alpha(alpha_)
			, beta(beta_)
			{}
		};
		
		NodeFilter(Params const& params=Params(), std::string const& className="NodeFilter");
		
		Params params() const;
		void setParams(Params const& params);
		
	protected:
		virtual void run();
		
	private:
		Params mParams;
		InletPtr mInlet;
		OutletPtr mOutlet;
		/// Keep a separete filter for each type
		std::unique_ptr<FilterSavitzky<Value>> mFilterValue;
		std::unique_ptr<FilterSavitzky<Point3d>> mFilterPoint3d;
		// Guards params
		mutable boost::shared_mutex mMutex;
		double mDataTimestamp;
	};
}