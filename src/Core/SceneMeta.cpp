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

SceneMetaPtr SceneMeta::defaultSceneMeta()
{
	static const SceneMetaPtr s(new SceneMeta);
	return s;
}

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


void SceneMeta::setupWindowLetterbox(int pixelWidth, int pixelHeight) const
{
	gl::setMatricesWindow(pixelWidth, pixelHeight);
	
	if (viewportSize != Vec2f())
	{
		
		Vec2f const& inputSize = viewportSize;
		Vec2f outputSize(pixelWidth, pixelHeight);
		
		// Calculate the offset and scale required to be able to draw to (0,0)-
		// mViewportSize and have the output letterboxed within the actual viewport
		Vec2f offset;
		float scale(1);
		// input data aspect
		float inputAspect = inputSize.x / inputSize.y;
		// viewport aspect
		float outputAspect = outputSize.x / outputSize.y;
		// if input is wider than output
		if (inputAspect > outputAspect)
		{
			// requires letterboxes on top and bottom
			scale = outputSize.x / inputSize.x;
			offset.y = int((outputSize.y - scale * inputSize.y) / 2);
		}
		else
		{
			// requires letterboxes on left and right
			scale = outputSize.y / inputSize.y;
			offset.x = int((outputSize.x - scale * inputSize.x) / 2);
		}
		gl::translate(offset);
		gl::scale(scale, scale);
	}
}





