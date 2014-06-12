#pragma once

#include "List.h"
#include "Point3d.h"
#include "Value.h"


namespace hm
{
//	/// Class representing a list of Point3ds
//	class ListPoint3d : public List<Point3d>
//	{
//	public:
//		typedef List<Point3d> BaseType;
//		
//		explicit
//		ListPoint3d(double timestamp = 0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
//		ListPoint3d(std::vector<Point3d> points, double timestamp=0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
//		
//		std::vector<Point3d> value;
//		
//		virtual Type type() const override { return LIST_POINT3D; }
//		
////		ListPoint3d& operator*=(Value const& rhs);
////		ListPoint3d& operator/=(Value const& rhs);
////		ListPoint3d operator*(Value const& rhs) const;
////		ListPoint3d operator/(Value const& rhs) const;
//		hm_list_define_mult_scalar_operators_and_assigns(ListPoint3d)
//	};
//
//hm_list_define_free_mult_scalar_operators(ListPoint3d);
	typedef List<Point3d> ListPoint3d;
}