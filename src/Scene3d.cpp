#include "Scene3d.h"
#include "Value.h"

using namespace hm;
using namespace std;

std::ostream& Scene3d::printTo(std::ostream& out) const
	{
		out << "Scene metaPtr:"<<sceneMeta<<" skels:"<<skeletons.size()<<" [";
		if (!skeletons.empty())
			out << '\n';
		for (Skeleton3d const& skel: skeletons)
		{
			out << skel << '\n';
		}
		return out << ']';
	}


Scene3d::Scene3d(double timestamp, SceneMetaPtr sceneMeta_)
: Base3dData(timestamp, sceneMeta_)
{
}



void Scene3d::draw()
{
	for (Skeleton3d& s: skeletons)
	{
		s.draw();
	}
}



Scene3d Scene3d::operator+(Scene3d const& rhs) const
{
	Scene3d out(max(timestamp, rhs.timestamp), sceneMeta);
	int n = std::min(skeletons.size(), rhs.skeletons.size());
	out.skeletons.reserve(skeletons.size());
	for (int i=0; i<n; i++)
	{
		out.skeletons.push_back(skeletons[i] + rhs.skeletons[i]);
	}
	// add over any skeletons we have that have no corresponding one in rhs
	for (int i=rhs.skeletons.size(); i<skeletons.size(); i++)
	{
		out.skeletons.push_back(skeletons[i]);
	}
	return out;
}

Scene3d Scene3d::operator-(Scene3d const& rhs) const
{
	Scene3d out(max(timestamp, rhs.timestamp), sceneMeta);
	int n = std::min(skeletons.size(), rhs.skeletons.size());
	out.skeletons.reserve(skeletons.size());
	for (int i=0; i<n; i++)
	{
		out.skeletons.push_back(skeletons[i] - rhs.skeletons[i]);
	}
	// add over any skeletons we have that have no corresponding one in rhs
	for (int i=rhs.skeletons.size(); i<skeletons.size(); i++)
	{
		out.skeletons.push_back(skeletons[i]);
	}
	return out;
}

Scene3d Scene3d::operator*(Scene3d const& rhs) const
{
	Scene3d out(max(timestamp, rhs.timestamp), sceneMeta);
	int n = std::min(skeletons.size(), rhs.skeletons.size());
	out.skeletons.reserve(skeletons.size());
	for (int i=0; i<n; i++)
	{
		out.skeletons.push_back(skeletons[i] * rhs.skeletons[i]);
	}
	// add over any skeletons we have that have no corresponding one in rhs
	for (int i=rhs.skeletons.size(); i<skeletons.size(); i++)
	{
		out.skeletons.push_back(skeletons[i]);
	}
	return out;
}

Scene3d Scene3d::operator/(Scene3d const& rhs) const
{
	Scene3d out(max(timestamp, rhs.timestamp), sceneMeta);
	int n = std::min(skeletons.size(), rhs.skeletons.size());
	out.skeletons.reserve(skeletons.size());
	for (int i=0; i<n; i++)
	{
		out.skeletons.push_back(skeletons[i] / rhs.skeletons[i]);
	}
	// add over any skeletons we have that have no corresponding one in rhs
	for (int i=rhs.skeletons.size(); i<skeletons.size(); i++)
	{
		out.skeletons.push_back(skeletons[i]);
	}
	return out;
}

Scene3d& Scene3d::operator+=(Scene3d const& rhs)
{
	timestamp = max(timestamp, rhs.timestamp);
	int n = std::min(skeletons.size(), rhs.skeletons.size());
	for (int i=0; i<n; i++)
	{
		skeletons[i] += rhs.skeletons[i];
	}
	return *this;
}

Scene3d& Scene3d::operator-=(Scene3d const& rhs)
{
	timestamp = max(timestamp, rhs.timestamp);
	int n = std::min(skeletons.size(), rhs.skeletons.size());
	for (int i=0; i<n; i++)
	{
		skeletons[i] -= rhs.skeletons[i];
	}
	return *this;
}

Scene3d& Scene3d::operator*=(Scene3d const& rhs)
{
	timestamp = max(timestamp, rhs.timestamp);
	int n = std::min(skeletons.size(), rhs.skeletons.size());
	for (int i=0; i<n; i++)
	{
		skeletons[i] *= rhs.skeletons[i];
	}
	return *this;
}

Scene3d& Scene3d::operator/=(Scene3d const& rhs)
{
	timestamp = max(timestamp, rhs.timestamp);
	int n = std::min(skeletons.size(), rhs.skeletons.size());
	for (int i=0; i<n; i++)
	{
		skeletons[i] /= rhs.skeletons[i];
	}
	return *this;
}

Scene3d Scene3d::operator+(Skeleton3d const& rhs) const
{
	Scene3d out = *this;
	out.timestamp = max(timestamp, rhs.timestamp);
	for (Skeleton3d& s: out.skeletons)
	{
		s += rhs;
	}
	return out;
}

Scene3d Scene3d::operator-(Skeleton3d const& rhs) const
{
	Scene3d out = *this;
	out.timestamp = max(timestamp, rhs.timestamp);
	for (Skeleton3d& s: out.skeletons)
	{
		s -= rhs;
	}
	return out;
}

Scene3d Scene3d::operator*(Skeleton3d const& rhs) const
{
	Scene3d out = *this;
	out.timestamp = max(timestamp, rhs.timestamp);
	for (Skeleton3d& s: out.skeletons)
	{
		s *= rhs;
	}
	return out;
}

Scene3d Scene3d::operator/(Skeleton3d const& rhs) const
{
	Scene3d out = *this;
	out.timestamp = max(timestamp, rhs.timestamp);
	for (Skeleton3d& s: out.skeletons)
	{
		s /= rhs;
	}
	return out;
}

Scene3d Scene3d::operator+(Point3d const& rhs) const
{
	Scene3d out = *this;
	out.timestamp = max(timestamp, rhs.timestamp);
	for (Skeleton3d& s: out.skeletons)
	{
		s = s + rhs;
	}
	return out;
}

Scene3d Scene3d::operator-(Point3d const& rhs) const
{
	Scene3d out = *this;
	out.timestamp = max(timestamp, rhs.timestamp);
	for (Skeleton3d& s: out.skeletons)
	{
		s = s - rhs;
	}
	return out;
}

Scene3d Scene3d::operator*(Point3d const& rhs) const
{
	Scene3d out = *this;
	out.timestamp = max(timestamp, rhs.timestamp);
	for (Skeleton3d& s: out.skeletons)
	{
		s *= rhs;
	}
	return out;
}

Scene3d Scene3d::operator/(Point3d const& rhs) const
{
	Scene3d out = *this;
	out.timestamp = max(timestamp, rhs.timestamp);
	for (Skeleton3d& s: out.skeletons)
	{
		s /= rhs;
	}
	return out;
}




Scene3d Scene3d::operator*(Value const& rhs) const
{
	Scene3d out(max(timestamp, rhs.timestamp), sceneMeta);
	for (Skeleton3d& s: out.skeletons)
	{
		out.skeletons.push_back(s * rhs);
	}
	return out;
}

Scene3d Scene3d::operator/(Value const& rhs) const
{
	Scene3d out(max(timestamp, rhs.timestamp), sceneMeta);
	for (Skeleton3d& s: out.skeletons)
	{
		out.skeletons.push_back(s / rhs);
	}
	return out;
}




bool Scene3d::operator==(Scene3d const& rhs) const
{
	if (sceneMeta != rhs.sceneMeta || skeletons.size() != rhs.skeletons.size())
		return false;
	for (int i=0; i<skeletons.size(); i++)
	{
		if (skeletons[i] != rhs.skeletons[i])
			return false;
	}
	return true;
}

