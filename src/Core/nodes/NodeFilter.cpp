//
//  NodeFilter.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 24/10/2013.
//
//

#include "nodes/NodeFilter.h"
#include "nodes/Filter.h"
#include "Inlet.h"
#include "Outlet.h"
#include <boost/thread/shared_lock_guard.hpp>
using namespace boost;

using namespace hm;
using namespace std;


#define hm_nodefilter_filters_define_visitation(Type) \
void NodeFilter::Filters::operator()(Type & x) \
{ \
x = m##Type.update(x); \
}

hm_nodefilter_filters_define_visitation(Value)
hm_nodefilter_filters_define_visitation(Point2d)
hm_nodefilter_filters_define_visitation(Point3d)
hm_nodefilter_filters_define_visitation(Skeleton3d)
hm_nodefilter_filters_define_visitation(Image2d)

// Lists require a bit more care, because the size/ids of skeletons may
// change. When this happens we reset the filter
#define hm_nodefilter_filters_define_list_visitation(Type) \
void NodeFilter::Filters::operator()(Type & x) \
{ \
	vector<int> ids; \
	ids.reserve(x.value.size()); \
	for (auto const& s: x.value) \
	{ \
		ids.push_back(s.id); \
	} \
	if (ids != mPrev##Type##Ids) \
	{ \
		m##Type.reset(x); \
		mPrev##Type##Ids = std::move(ids); \
		hm_debug("List size changed: resetting filter."); \
	} \
	x = m##Type.update(x); \
}

hm_nodefilter_filters_define_list_visitation(ListValue);
hm_nodefilter_filters_define_list_visitation(ListPoint2d);
hm_nodefilter_filters_define_list_visitation(ListPoint3d);
hm_nodefilter_filters_define_list_visitation(Scene3d);




NodeFilter::NodeFilter(Params const& params, std::string const& className)
: NodeUnthreaded(params, className, "Filter", "Smooths data.")
, mAlpha(0.5f)
, mBeta(0.5f)
, mInlet(nullptr)
, mOutlet(nullptr)
, mDataTimestamp(-42)
{
	mInlet = createInlet(ALL_TYPES, "Data to filter", "Values received are filtered and sent out");
	mOutlet = createOutlet(ALL_TYPES, "Filtered data", "");
	
	auto a = addParameter("Alpha", &mAlpha, "First order smoothing on the filter");
	a->setBounds(0, 1, 0, 1);
	a->addNewExternalValueCallback([this]() { callbackAlphaBetaChanged(); });
	
	auto b = addParameter("Beta", &mBeta, "Second order smoothing on the filter");
	b->setBounds(0, 1, 0, 1);
	b->addNewExternalValueCallback([this]() { callbackAlphaBetaChanged(); });
}


NodePtr NodeFilter::create(Params params) const
{
	return NodePtr(new NodeFilter(params));
}


void NodeFilter::callbackAlphaBetaChanged()
{
#define hm_nodefilter_set_alpha_beta(filterName) \
	mFilters.filterName.predictor.alpha = mAlpha; \
	mFilters.filterName.predictor.beta = mBeta;
	
	hm_nodefilter_set_alpha_beta(mValue);
	hm_nodefilter_set_alpha_beta(mListValue);
	hm_nodefilter_set_alpha_beta(mPoint2d);
	hm_nodefilter_set_alpha_beta(mListPoint2d);
	hm_nodefilter_set_alpha_beta(mPoint3d);
	hm_nodefilter_set_alpha_beta(mListPoint3d);
	hm_nodefilter_set_alpha_beta(mSkeleton3d);
	hm_nodefilter_set_alpha_beta(mScene3d);
	hm_nodefilter_set_alpha_beta(mImage2d);
}




void NodeFilter::step()
{
	Data data = mInlet->dataIfNewerThan(mDataTimestamp);
	if (!data.isNull())
	{
		Data data = mInlet->data();
		mDataTimestamp = data.timestamp();
		boost::apply_visitor(mFilters, data.data);
//		switch (data.type())
//		{
//			case VALUE:
//			{
//				Value& x = data.asValue();
//				x = mFilterValue->update(x);
//				break;
//			}
//			case POINT3D:
//			{
//				Point3d& x = data.asPoint3d();
//				x = mFilterPoint3d->update(x);
//				break;
//			}
//			case SKELETON3D:
//			{
//				Skeleton3d& x = data.asSkeleton3d();
//				x = mFilterSkeleton3d->update(x);
//				break;
//			}
//			case SCENE3D:
//			{
//				Scene3d& x = data.asScene3d();
//				x = mFilterScene3d->update(x);
//				break;
//			}
//			default:
//				// unsupported types
//				hm_error("Unsupported data type");
//				assert(false);
//				
//		}
		assert(!data.isNull());
		mOutlet->outputNewData(data);
	}
}



