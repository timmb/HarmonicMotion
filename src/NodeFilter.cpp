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
: Node(className)
, mInlet(new Inlet(VALUE | POINT3D, "Values to filter", "Values received are filtered and sent out"))
, mOutlet(new Outlet(VALUE | POINT3D, "Filtered values", ""))
, mFilterValue(new FilterSavitzky<Value>)
, mFilterPoint3d(new FilterSavitzky<Point3d>)
, mDataTimestamp(-42)
{
}


NodeFilter::Params NodeFilter::params() const
{
	shared_lock<shared_mutex> lock(mMutex);
	return mParams;
}

void NodeFilter::setParams(Params const& params)
{
	shared_lock<shared_mutex> lock(mMutex);
	mParams = params;
}

void NodeFilter::run()
{
	while (!isRequestedToStop())
	{
		if (mInlet->dataTimestamp() > mDataTimestamp)
		{
			Data data = mInlet->data();
			mDataTimestamp = data.timestamp();
			switch (data.type())
			{
				case VALUE:
				{
					Value& x = data.asValue();
					x = mFilterValue->update(x);
					break;
				}
				case POINT3D:
				{
					Point3d& x = data.asPoint3d();
					x = mFilterPoint3d->update(x);
					break;
				}
				default:
					// unsupported types
					std::cerr << "Unsupported data type"<<std::endl;
					assert(false);
					continue;
					
			}
			mOutlet->outputNewData(data);
		}
		waitForNewData();
	}
}