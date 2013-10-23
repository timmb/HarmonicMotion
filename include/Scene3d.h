#pragma once
#include "Common.h"
#include "Skeleton3d.h"

namespace hm
{
	/// Class that stores a set of Skeleton3d's
	class Scene3d : public DataType
	{
	public:
		Scene3d();
		
		std::vector<Skeleton3d>& skeletons() { return mSkeletons; }
		std::vector<Skeleton3d> const& skeletons() const { return mSkeletons; }
		
		virtual std::ostream& printTo(std::ostream& out) const override;
		virtual Type type() const override { return SCENE3D; }
		virtual void draw() override;
		
	private:
		std::vector<Skeleton3d> mSkeletons;
	};
}