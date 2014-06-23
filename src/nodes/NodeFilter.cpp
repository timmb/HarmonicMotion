//
//  NodeFilter.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 24/10/2013.
//
//

#include "NodeFilter.h"
#include "Filter.h"
#include "Inlet.h"
#include "Outlet.h"
#include <boost/thread/shared_lock_guard.hpp>
using namespace boost;

using namespace hm;





NodeFilter::NodeFilter(Params const& params, std::string const& className)
: NodeUnthreaded(params, className)
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
	
	auto b = addParameter("Beta", &mBeta, "Second order smoothing on the filter");
	b->setBounds(0, 1, 0, 1);
}


NodePtr NodeFilter::create(Params params) const
{
	return NodePtr(new NodeFilter(params));
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



