#include "WidgetRenderView.h"
#include "nodes/NodeRenderer.h"
#include <QBoxLayout>
#include <QLabel>
#include "Utilities.h"
#include "cinder/gl/gl.h"

using namespace std;
using namespace hm;


WidgetRenderView::WidgetRenderView(NodeRendererPtr node, QWidget* parent)
: QWidget(parent)
, mGlWidget(new WidgetRenderViewGl(node, this))
{
	QVBoxLayout* layout = new QVBoxLayout(this);
//	layout->addWidget(new QLabel(str(node->name())));
	layout->addWidget(mGlWidget);
	setLayout(layout);
	
	
}

WidgetRenderView::~WidgetRenderView()
{
	hm_debug("WidgetRenderView::~WidgetRenderView. this: "<<this<<" parent: "<<parent());
}

NodeRendererPtr WidgetRenderView::node() const
{
	return mGlWidget->node();
}

void WidgetRenderView::setNode(NodeRendererPtr node)
{
	mGlWidget->setNode(node);
}



WidgetRenderViewGl::WidgetRenderViewGl(NodeRendererPtr node, QWidget* parent)
: QGLWidget(parent)
, mForceRedraw(false)
, mNode(node)
, mTimer(new QTimer(this))
, mMaxFrameRate(30)
{
	assert(mNode != nullptr);
	mTimer->setInterval(1000 / mMaxFrameRate);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(timerCallback()));
}

WidgetRenderViewGl::~WidgetRenderViewGl()
{
	hm_debug("~WidgetRenderViewGl::~~WidgetRenderViewGl. this: "<<this<<" parent: "<<parent());
	mTimer->stop();
	// This prevents memory corruption with Qt 5.2.1 by the QGLWidget
	// destructor.
	// https://bugreports.qt-project.org/browse/QTBUG-36820
	doneCurrent();
}

void WidgetRenderViewGl::setNode(NodeRendererPtr node)
{
	mNode = node;
	mForceRedraw = true;
	mTimer->start();
}

NodeRendererPtr WidgetRenderViewGl::node() const
{
	return mNode;
}

void WidgetRenderViewGl::initializeGL()
{
	mTimer->start();
}

void WidgetRenderViewGl::timerCallback()
{
	mTimer->stop();
	if (mNode->isRedrawRequired() || mForceRedraw)
	{
		updateGL();
	}
	if (mNode != nullptr)
	{
		mTimer->start();
	}
}


void WidgetRenderViewGl::paintGL()
{
	if (mNode==nullptr)
	{
		ci::gl::clear(ci::ColorA::black());
	}
	else
	{
		mNode->draw(width(), height());
	}
}

QSize WidgetRenderViewGl::sizeHint() const
{
	return QSize(400, 250);
}



