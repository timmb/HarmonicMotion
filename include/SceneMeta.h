//
//  SceneMeta.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 23/10/2013.
//
//

#pragma once

#include "cinder/Vector.h"

namespace hm
{
	class SceneMeta;
	/// This is a const pointer to allow thread-safe access
	typedef std::shared_ptr<const SceneMeta> SceneMetaPtr;
	
	/// Metadata describing a 3D scene indicating camera position, etc.
	class SceneMeta
	{
	public:
		/// Location of camera in world coordinates
		ci::Vec3f cameraPos;
		/// Unit vector in direction camera is pointing
		ci::Vec3f cameraDir;
		/// Field of view of camera in degrees
		double cameraFov;
		
		SceneMeta()
		: cameraPos(0,0,0)
		, cameraDir(0,0,1)
		, cameraFov(57)
		{}
		
		/// Sets up the current GL context to use the camera defined by this
		/// metadata
		void setupCamera(int pixelWidth, int pixelHeight) const;
		
		static const SceneMetaPtr sDefaultSceneMeta;
		
	};
	
	// Chooses a SceneMetaPtr, opting for the non-default one if present,
	// prioritising \p x if both are non-default
	inline SceneMetaPtr choose(SceneMetaPtr const& x, SceneMetaPtr const& y)
	{
		return x==SceneMeta::sDefaultSceneMeta? y : x;
	}
}