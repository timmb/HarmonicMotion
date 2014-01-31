//
//  NodeFilter.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 24/10/2013.
//
//

#pragma once

#include "NodeThreaded.h"
#include "nodes/Filter.h"
#include <boost/thread/shared_mutex.hpp>

namespace hm
{
	class NodeFilter : public NodeThreaded
	{
	public:
		struct Params : public Node::Params
		{
			/// First order smoothing on the Predictor
			float alpha;
			/// Second order smoothing on the Predictor
			float beta;
			Params(float alpha_=0.5f, float beta_=0.5f, std::string const& name="")
			: Node::Params(name)
			, alpha(alpha_)
			, beta(beta_)
			{}
		};
		
		NodeFilter(Params const& params=Params(), std::string const& className="NodeFilter");
		virtual ~NodeFilter();
		
		Params params() const;
		void setParams(Params params);
		
	protected:
		virtual void run();
		
	private:
		Params mParams;
		InletPtr mInlet;
		OutletPtr mOutlet;
		/// Keep a separete filter for each type
		std::unique_ptr<FilterSavitzky<Value>> mFilterValue;
		std::unique_ptr<FilterSavitzky<Point3d>> mFilterPoint3d;
		std::unique_ptr<FilterSavitzky<Skeleton3d>> mFilterSkeleton3d;
		std::unique_ptr<FilterSavitzky<Scene3d>> mFilterScene3d;
		// Guards params
		mutable boost::shared_mutex mMutex;
		double mDataTimestamp;
	};
}