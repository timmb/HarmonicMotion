//
//  SceneMeta.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 23/10/2013.
//
//

#include "SceneMeta.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"

using namespace hm;
using namespace ci;

void SceneMeta::setupCamera(int pixelWidth, int pixelHeight) const
{
	// our coordinates are in metres so this should suffice
	float nearPlane = 0.05f;
	float farPlane = 500.f;
	CameraPersp cam(pixelWidth, pixelHeight, cameraFov, nearPlane, farPlane);
	cam.setEyePoint(cameraPos);
	cam.setViewDirection(cameraDir);
	gl::setMatrices(cam);
}