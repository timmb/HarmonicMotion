//
//  NodeRendererGlWidget.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "WidgetMonitor.h"
#include "nodes/NodeRenderer.h"

using namespace hm;

NodeRendererGlWidget::NodeRendererGlWidget(QWidget* parent)
: QGLWidget(parent)
, mMaxFrameRate(30)
, mRenderer(nullptr)
, mRedrawTimer(new QTimer(this))
{
	mRedrawTimer->setInterval(1000 / mMaxFrameRate);
	connect(mRedrawTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
}

void NodeRendererGlWidget::setRenderer(std::shared_ptr<NodeRenderer> renderer)
{
	mRenderer = renderer;
}

void NodeRendererGlWidget::initializeGL()
{
	mRedrawTimer->start();
	resizeGL(width(), height());
}

void NodeRendererGlWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
}

void NodeRendererGlWidget::paintGL()
{
	mRedrawTimer->blockSignals(true);
	if (true)//(mRenderer->isRedrawRequired())
	{
		mRenderer->draw(width(), height());
	}
	mRedrawTimer->blockSignals(false);
}

QSize NodeRendererGlWidget::sizeHint() const
{
	return QSize(400, 300);
}