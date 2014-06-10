#pragma once

#include "List.h"
#include "Point3d.h"

namespace hm
{
	/// Class representing a list of Point3ds
	class ListPoint3d : public List<Point3d>
	{
	public:
		typedef List<Point3d> BaseType;
		
		explicit
		ListPoint3d(double timestamp = 0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
		ListPoint3d(std::vector<Point3d> points, double timestamp=0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
		
		std::vector<Point3d> value;
		
		virtual Type type() const override { }
		
		ListPoint3d& operator*=(Value const& rhs);
		ListPoint3d& operator/=(Value const& rhs);
		ListPoint3d operator*(Value const& rhs) const;
		ListPoint3d operator/(Value const& rhs) const;
		
	};
	
}