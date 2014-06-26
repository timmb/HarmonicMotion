#pragma once
#include "Common.h"
#include "Skeleton3d.h"
#include "SceneMeta.h"
#include "List.h"

namespace hm
{
	// In future this might be expanded to a proper class that includes
	// information about the scene as well as the skeletons within it.
	typedef List<Skeleton3d> Scene3d;
	
	// We additionally define operators +,-,*,/,+=,etc for Scene3d against Point3d
	
#define hm_scene3d_define_free_right_point3d_op_assign(op_assign) \
	inline Scene3d& operator op_assign(Scene3d & lhs, Point3d const& rhs) \
	{ \
		for (Skeleton3d & x: lhs.value) \
		{ \
			x op_assign rhs; \
		} \
		return lhs; \
	}
	
#define hm_scene3d_define_free_right_point3d_op(op) \
	inline Scene3d operator op(Scene3d lhs, Point3d const& rhs) \
	{ \
		return lhs op##= rhs; \
	}
	
	hm_scene3d_define_free_right_point3d_op_assign(+=)
	hm_scene3d_define_free_right_point3d_op_assign(-=)
	hm_scene3d_define_free_right_point3d_op_assign(*=)
	hm_scene3d_define_free_right_point3d_op_assign(/=)
	hm_scene3d_define_free_right_point3d_op(+)
	hm_scene3d_define_free_right_point3d_op(-)
	hm_scene3d_define_free_right_point3d_op(*)
	hm_scene3d_define_free_right_point3d_op(/)
	
	
#define hm_scene3d_define_free_left_point3d_op(op) \
inline Scene3d operator op(Point3d const& lhs, Scene3d rhs) \
{ \
	for (Skeleton3d & x: rhs.value) \
	{ \
		x op lhs; \
	} \
	return rhs; \
}
	
	hm_scene3d_define_free_left_point3d_op(+)
	hm_scene3d_define_free_left_point3d_op(-)
	hm_scene3d_define_free_left_point3d_op(*)
	hm_scene3d_define_free_left_point3d_op(/)
	
	
	
//	/// Class that stores a set of Skeleton3d's
//	class Scene3d : public Base3dData
//	{
//	public:
//		typedef Base3dData BaseType;
//		
//		explicit
//		Scene3d(double timestamp = 0., SceneMetaPtr sceneMeta = SceneMeta::defaultSceneMeta());
//		
//		std::vector<Skeleton3d> skeletons;
//		
//		virtual std::ostream& printTo(std::ostream& out) const override;
//		virtual Type type() const override { return SCENE3D; }
//		virtual void draw() override;
//		
//		Scene3d operator+(Scene3d const& rhs) const;
//		Scene3d operator-(Scene3d const& rhs) const;
//		Scene3d operator*(Scene3d const& rhs) const;
//		Scene3d operator/(Scene3d const& rhs) const;
//
//		Scene3d& operator+=(Scene3d const& rhs);
//		Scene3d& operator-=(Scene3d const& rhs);
//		Scene3d& operator*=(Scene3d const& rhs);
//		Scene3d& operator/=(Scene3d const& rhs);
//		
//		/// rhs is applied to each skeleton in this scene
//		Scene3d operator+(Skeleton3d const& rhs) const;
//		Scene3d operator-(Skeleton3d const& rhs) const;
//		Scene3d operator*(Skeleton3d const& rhs) const;
//		Scene3d operator/(Skeleton3d const& rhs) const;
//		
//		/// rhs is applied to each joint of each skeleton in this scene
//		Scene3d operator+(Point3d const& rhs) const;
//		Scene3d operator-(Point3d const& rhs) const;
//		Scene3d operator*(Point3d const& rhs) const;
//		Scene3d operator/(Point3d const& rhs) const;
//
//		/// rhs is applied to each component of each joint of each skeleton in this scene
//		Scene3d operator*(Value const& rhs) const;
//		Scene3d operator/(Value const& rhs) const;
//
//		inline
//		Scene3d operator*(double rhs) const;
//		inline
//		Scene3d operator/(double rhs) const;
//		
//		inline
//		Scene3d& operator*=(double rhs);
//		inline
//		Scene3d& operator/=(double rhs);
//		
//		bool operator==(Scene3d const& rhs) const;
//		bool operator!=(Scene3d const& rhs) const { return !(*this==rhs); }
//
//		Scene3d operator+() const { return *this; }
//		Scene3d operator-() const;
//		
//	private:
//	};
//	
//	
//	inline
//	Scene3d operator*(double lhs, Scene3d const& rhs)
//	{
//		return rhs * lhs;
//	}
//	
//	inline
//	Scene3d Scene3d::operator*(double rhs) const
//	{
//		Scene3d out = *this;
//		for (Skeleton3d& s: out.skeletons)
//		{
//			s *= rhs;
//		}
//		return out;
//	}
//	
//	inline
//	Scene3d Scene3d::operator/(double rhs) const
//	{
//		Scene3d out = *this;
//		for (Skeleton3d& s: out.skeletons)
//		{
//			s /= rhs;
//		}
//		return out;
//	}
//	
//	
//	inline
//	Scene3d& Scene3d::operator*=(double rhs)
//	{
//		for (auto& skel: skeletons)
//		{
//			skel *= rhs;
//		}
//		return *this;
//	}
//	
//	inline
//	Scene3d Scene3d::operator-() const
//	{
//		Scene3d out(timestamp, sceneMeta);
//		out.skeletons.reserve(skeletons.size());
//		for (Skeleton3d const& s: skeletons)
//		{
//			out.skeletons.push_back(-s);
//		}
//		return out;
//	}

}