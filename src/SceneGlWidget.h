//
//  SceneGlWidget.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once
#include <QGLWidget>

namespace hm
{
	class SceneGlWidget : public QGLWidget
	{
	public:
		SceneGlWidget(QWidget* parent = nullptr);
	}
}