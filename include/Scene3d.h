#pragma once
#include "Common.h"
#include "Skeleton3d.h"
#include "SceneMeta.h"

namespace hm
{
	/// Class that stores a set of Skeleton3d's
	class Scene3d : public DataType
	{
	public:
		Scene3d(SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
		
		std::vector<Skeleton3d>& skeletons() { return mSkeletons; }
		std::vector<Skeleton3d> const& skeletons() const { return mSkeletons; }
		
		virtual std::ostream& printTo(std::ostream& out) const override;
		virtual Type type() const override { return SCENE3D; }
		virtual void draw() override;
		
		/// Scenes must always have scene metadata
		virtual bool hasSceneMeta() const override;
		
		Scene3d operator+(Scene3d rhs) const;
		Scene3d operator-(Scene3d rhs) const;
		Scene3d& operator+=(Scene3d const& rhs);
		
		template <typename Scalar>
		Scene3d operator*(Scalar const& rhs) const;
		
		template <typename Scalar>
		Scene3d& operator*=(Scalar const& rhs);
		
		bool operator==(Scene3d const& rhs) const;
		bool operator!=(Scene3d const& rhs) const { return !(*this==rhs); }

		
	private:
		std::vector<Skeleton3d> mSkeletons;
	};
	
	
	template <typename T>
	Scene3d operator*(T const& lhs, Scene3d const& rhs)
	{
		return rhs * lhs;
	}
	
	template <typename Scalar>
	Scene3d Scene3d::operator*(Scalar const& rhs) const
	{
		Scene3d scene = *this;
		scene *= rhs;
		return scene;
	}
	
	
	template <typename Scalar>
	Scene3d& Scene3d::operator*=(Scalar const& rhs)
	{
		for (auto& skel: mSkeletons)
		{
			skel *= rhs;
		}
		return *this;
	}
	


}