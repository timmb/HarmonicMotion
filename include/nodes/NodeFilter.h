//
//  NodeFilter.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 24/10/2013.
//
//

#pragma once

#include "NodeUnthreaded.h"
#include "nodes/Filter.h"
#include <boost/thread/shared_mutex.hpp>
#include "Data.h"

namespace hm
{
	class NodeFilter : public NodeUnthreaded
	{
	public:
		NodeFilter(Params const& params=Params(), std::string const& className="NodeFilter");
		
		Params params() const;
		void setParams(Params params);
		
	protected:
		virtual NodePtr create(Params params) const override;
		virtual void step() override;
		
	private:
		/// Keep a separete filter for each type
		struct Filters : public boost::static_visitor<void>
		{
			FilterSavitzky<Value, double> mValue;
			FilterSavitzky<ListValue, double> mListValue;
			FilterSavitzky<Point2d, float> mPoint2d;
			FilterSavitzky<ListPoint2d, float> mListPoint2d;
			FilterSavitzky<Point3d, float> mPoint3d;
			FilterSavitzky<ListPoint3d, float> mListPoint3d;
			FilterSavitzky<Skeleton3d, float> mSkeleton3d;
			FilterSavitzky<Scene3d, float> mScene3d;
			FilterSavitzky<Image2d, float> mImage2d;
			
			void operator()(Value& x);
			void operator()(ListValue& x);
			void operator()(Point2d& x);
			void operator()(ListPoint2d& x);
			void operator()(Point3d& x);
			void operator()(ListPoint3d& x);
			void operator()(Skeleton3d& x);
			void operator()(Scene3d& x);
			void operator()(Image2d& x);
			
			void operator()(DataNull & d) {}
			
		private:
			std::vector<int> mPrevListValueIds;
			std::vector<int> mPrevListPoint2dIds;
			std::vector<int> mPrevListPoint3dIds;
			std::vector<int> mPrevScene3dIds;
		};
		
		void callbackAlphaBetaChanged();
		
		/// First order smoothing on the Predictor
		float mAlpha;
		/// Second order smoothing on the Predictor
		float mBeta;
		InletPtr mInlet;
		OutletPtr mOutlet;
		Filters mFilters;
		
//		std::unique_ptr<FilterSavitzky<Data>> mFilterData;
////		std::unique_ptr<FilterSavitzky<Value>> mFilterValue;
////		std::unique_ptr<FilterSavitzky<Point3d>> mFilterPoint3d;
////		std::unique_ptr<FilterSavitzky<Skeleton3d>> mFilterSkeleton3d;
////		std::unique_ptr<FilterSavitzky<Scene3d>> mFilterScene3d;
		double mDataTimestamp;
//		Type mLastDataType;
	};
}

#include "FactoryNode.h"
hm_register_node(NodeFilter, "Filter", "Smooths data.");








