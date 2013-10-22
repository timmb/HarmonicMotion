#include "Scene3d.h"
#include <sstream>

namespace hm
{
	std::ostream& operator<<(std::ostream& out, Scene3d const& rhs)
	{
		out << "Scene skels:"<<rhs.skeletons().size()<<" [";
		if (!rhs.skeletons().empty())
			out << '\n';
		for (Skeleton3d const& skel: rhs.skeletons())
		{
			out << skel << '\n';
		}
		return out << ']';
	}
}

using namespace hm;

Scene3d::Scene3d()
{}

std::string Scene3d::toString() const
{
	return (std::stringstream() << *this).str();
}

