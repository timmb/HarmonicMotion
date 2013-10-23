#include "Scene3d.h"

using namespace hm;

std::ostream& Scene3d::printTo(std::ostream& out) const
	{
		out << "Scene skels:"<<skeletons().size()<<" [";
		if (!skeletons().empty())
			out << '\n';
		for (Skeleton3d const& skel: skeletons())
		{
			out << skel << '\n';
		}
		return out << ']';
	}


Scene3d::Scene3d(SceneMetaPtr sceneMeta_)
: DataType(sceneMeta_)
{
	assert(sceneMeta != nullptr);
}



void Scene3d::draw()
{
	for (Skeleton3d& s: mSkeletons)
	{
		s.draw();
	}
}

bool Scene3d::hasSceneMeta() const
{
	assert(sceneMeta != nullptr);
	return true;
}