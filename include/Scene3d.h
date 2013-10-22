#pragma once
#include "Common.h"
#include "Skeleton3d.h"

namespace hm
{
	/// Class that stores a set of Skeleton3d's
	class Scene3d
	{
	public:
		Scene3d();
		
		std::vector<Skeleton3d>& skeletons() { return mSkeletons; }
		std::vector<Skeleton3d> const& skeletons() const { return mSkeletons; }
		
		std::string toString() const;
		
	private:
		std::vector<Skeleton3d> mSkeletons;
		
		friend std::ostream& operator<<(std::ostream&, Scene3d const& rhs);
	};
	
	std::ostream& operator<<(std::ostream&, Scene3d const&);
}