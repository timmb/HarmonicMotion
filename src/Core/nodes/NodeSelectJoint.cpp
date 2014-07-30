#include "nodes/NodeSelectJoint.h"
#include "Inlet.h"
#include "Outlet.h"

using namespace hm;
using namespace std;

NodeSelectJoint::NodeSelectJoint(Params const& params, string const& className)
: NodeUnthreaded(params, className, "Select joint", "Choose a joint from a skeleton.")
, mNumOutlets(1)
, mSelectedJoints(MAX_NUM_OUTLETS, 0)
, mLastTimestamp(-42)
{
	createInlet(SKELETON3D | SCENE3D | LIST_POINT2D | LIST_VALUE, "Skeleton input", "Skeletons received here will have the selected joint (3D point) outputted. If a scene is received then the output will be a list of 3D points, one for each skeleton in the scene.  You can also enter a list of values or 2D points and if there is an element with an ID matching the joint then that will be outputted.");
	auto p = addParameter("Number of outlets", &mNumOutlets, "If you would like to output more than one joint of the input then you can choose to have multiple outlets");
	p->setBounds(0, MAX_NUM_OUTLETS, 0, MAX_NUM_OUTLETS);
	
	assert(mSelectedJoints.size() == MAX_NUM_OUTLETS);
	for (int i=0; i<mSelectedJoints.size(); i++)
	{
		auto p = addParameter("Outlet "+to_string(i+1), &mSelectedJoints[i], "Select which joint to output at outlet "+to_string(i+1));
		p->setEnumerationLabels(JOINT_NAMES);
		mSelectedJointParameters.push_back(p);
	}
	assert(mSelectedJoints.size() == mSelectedJointParameters.size());
	setupOutlets();
}


NodePtr NodeSelectJoint::create(Params params) const
{
	return NodePtr(new NodeSelectJoint(params));
}


void NodeSelectJoint::setupOutlets()
{
	assert(0 <= mNumOutlets && mNumOutlets <= 15);
	while (numOutlets() > mNumOutlets)
	{
		removeOutlet(outlet(numOutlets()-1));
	}
	while (numOutlets() < mNumOutlets)
	{
		createOutlet(POINT3D | LIST_POINT3D | POINT2D | VALUE, "Outlet "+to_string(numOutlets()+1), "A 3D point for the chosen joint will be outputted here (or a list of 3D points if the input is a scene rather than a single skeleton).");
	}
	assert(numOutlets() == mNumOutlets);
	assert(mSelectedJoints.size() == mSelectedJointParameters.size());
	for (int i=0; i<mSelectedJointParameters.size(); i++)
	{
		mSelectedJointParameters[i]->setVisible(i < numOutlets());
	}
}


void NodeSelectJoint::step()
{
	setupOutlets();
	assert(numOutlets() == mNumOutlets);
	assert(mSelectedJoints.size() >= mNumOutlets);
	Data data = inlet(0)->dataIfNewerThan(mLastTimestamp);
	switch (data.type())
	{
		case SKELETON3D:
		{
			Skeleton3d s = data.asSkeleton3d();
			for (int i=0; i<mNumOutlets; i++)
			{
				Data data = s.joint(mSelectedJoints[i]);
				outlet(i)->outputNewData(data);
			}
		}
			break;
		case SCENE3D:
		{
			Scene3d s = data.asScene3d();
			for (int i=0; i<mNumOutlets; i++)
			{
				ListPoint3d out(s.timestamp, s.id, s.sceneMeta);
				out.value.reserve(s.value.size());
				for (Skeleton3d const& skel: s.value)
				{
					out.value.push_back(skel.joint(mSelectedJoints[i]));
				}
				Data data(out);
				outlet(i)->outputNewData(data);
			}
		}
			break;
		case LIST_POINT2D:
		{
			ListPoint2d s = data.asListPoint2d();
			for (int i=0; i<mNumOutlets; i++)
			{
				for (Point2d const& p: s.value)
				{
					if (p.id == mSelectedJoints[i])
					{
						Data data = s.value[mSelectedJoints[i]];
						outlet(i)->outputNewData(data);
						break;
					}
				}
			}
		}
			break;
		case LIST_VALUE:
		{
			ListValue s = data.asListValue();
			for (int i=0; i<mNumOutlets; i++)
			{
				for (Value const& p: s.value)
				{
					if (p.id == mSelectedJoints[i])
					{
						Data data = s.value[mSelectedJoints[i]];
						outlet(i)->outputNewData(data);
						break;
					}
				}
			}
		}
			break;
		default:
			break;
	}
	if (!data.isNull())
	{
		mLastTimestamp = data.timestamp();
	}
}









