//
//  NodeRendererGlWidget.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "NodeRendererGlWidget.h"
#include "NodeRenderer.h"

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
}

void NodeRendererGlWidget::paintGL()
{
	mRedrawTimer->blockSignals(true);
	if (true)//(mRenderer->isRedrawRequired())
	{
		mRenderer->draw();
	}
	mRedrawTimer->blockSignals(false);
}

QSize NodeRendererGlWidget::sizeHint() const
{
	return QSize(400, 300);
}