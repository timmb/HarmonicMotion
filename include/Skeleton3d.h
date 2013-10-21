#pragma once
#include "Common.h"


namespace hm
{
	class Skeleton3d
	{
	public:
		Skeleton3d();
		
	private:
		std::vector<Point3d> mJoints;
	};
}