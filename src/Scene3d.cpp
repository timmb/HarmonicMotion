#include "Scene3d.h"

using namespace hm;

std::ostream& Scene3d::printTo(std::ostream& out) const
	{
		out << "Scene metaPtr:"<<sceneMeta<<" skels:"<<skeletons().size()<<" [";
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


Scene3d Scene3d::operator+(Scene3d rhs) const
{
	int n = std::min(skeletons().size(), rhs.skeletons().size());
	for (int i=0; i<n; i++)
	{
		rhs.skeletons()[i] += skeletons()[i];
	}
	// add over any skeletons we have that have no corresponding one in rhs
	for (int i=rhs.skeletons().size(); i<skeletons().size(); i++)
	{
		rhs.skeletons().push_back(skeletons()[i]);
	}
	return rhs;
}

Scene3d Scene3d::operator-(Scene3d rhs) const
{
	int n = std::min(skeletons().size(), rhs.skeletons().size());
	for (int i=0; i<n; i++)
	{
		rhs.skeletons()[i] -= skeletons()[i];
	}
	// doesn't make sense to add over negative skeletons where there is no
	// correspondence
	return rhs;
}

Scene3d& Scene3d::operator+=(Scene3d const& rhs)
{
	int n = std::min(skeletons().size(), rhs.skeletons().size());
	for (int i=0; i<n; i++)
	{
		skeletons()[i] += rhs.skeletons()[i];
	}
	// add over any skeletons in rhs that have no corresponding in this
	for (int i=skeletons().size(); i<rhs.skeletons().size(); i++)
	{
		skeletons().push_back(rhs.skeletons()[i]);
	}
	return *this;

}


bool Scene3d::operator==(Scene3d const& rhs) const
{
	if (sceneMeta != rhs.sceneMeta || skeletons().size() != rhs.skeletons().size())
		return false;
	for (int i=0; i<mSkeletons.size(); i++)
	{
		if (skeletons()[i] != rhs.skeletons()[i])
			return false;
	}
	return true;
}

