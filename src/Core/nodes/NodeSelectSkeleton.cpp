#include "nodes/NodeSelectSkeleton.h"
#include <boost/assign/list_of.hpp>
#include "Inlet.h"
#include "Outlet.h"
#include "Scene3d.h"
#include "Functions.h"

using namespace hm;

NodeSelectSkeleton::NodeSelectSkeleton(Node::Params params, std::string className)
: NodeUnthreaded(params, className, "Select skeleton", "Choose a skeleton from a scene based on distance.")
, mMode(0)
, mLastDataTimestamp(-42.)
{
	auto p = addParameter("Mode", &mMode, "How to choose the skeleton from the scene. Currently only 'closest' is implemeted");
	p->setEnumerationLabels(boost::assign::list_of("Closest"));
	
	createInlet(SCENE3D, "Scene to select skeleton from","Input a Scene3d here that you would like to choose a Skeleton3d");
	createOutlet(SKELETON3D, "Selected skeleton", "The Skeleton3d that has been selected from the inputted scene.");
}

NodePtr NodeSelectSkeleton::create(Node::Params params) const
{
	return NodePtr(new NodeSelectSkeleton(params));
	
}

void NodeSelectSkeleton::step()
{
	assert(mMode == CLOSEST);
	Data data = inlet(0)->dataIfNewerThan(mLastDataTimestamp);
	if (!data.isNull())
	{
		if (data.isScene3d())
		{
			Scene3d scene = data.asScene3d();
			if (!scene.value.empty())
			{
				double closest = std::numeric_limits<double>::max();
				int closestArg = -1;
				for (int i=0; i<scene.value.size(); i++)
				{
					double dist = magnitude(scene.value[i].joint(TORSO)).value;
					if (dist < closest)
					{
						closest = dist;
						closestArg = i;
					}
					assert(closestArg != -1);
					Data output(scene.value[i]);
					outlet(0)->outputNewData(output);
				}
			}
		}
	}
}