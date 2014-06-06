#include "WidgetRenderView.h"
#include "nodes/NodeRenderer.h"
#include <QBoxLayout>
#include <QLabel>
#include "Utilities.h"

using namespace std;
using namespace hm;


WidgetRenderView::WidgetRenderView(shared_ptr<NodeRenderer> node, QWidget* parent)
: QWidget(parent)
, mNode(node)
{
	WidgetRenderViewGl* gl = new WidgetRenderViewGl(node, this);
	QVBoxLayout* layout = new QVBoxLayout(this);
//	layout->addWidget(new QLabel(str(node->name())));
	layout->addWidget(gl);
	setLayout(layout);
	
	
}



WidgetRenderViewGl::WidgetRenderViewGl(shared_ptr<NodeRenderer> node, QWidget* parent)
: QGLWidget(parent)
, mNode(node)
, mTimer(new QTimer(this))
, mMaxFrameRate(30)
{
	mTimer->setInterval(1000 / mMaxFrameRate);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(timerCallback()));
}

void WidgetRenderViewGl::initializeGL()
{
	mTimer->start();
}

void WidgetRenderViewGl::timerCallback()
{
	mTimer->stop();
	if (mNode->isRedrawRequired())
	{
		updateGL();
	}
	mTimer->start();
}


void WidgetRenderViewGl::paintGL()
{
	mNode->draw(width(), height());
}

QSize WidgetRenderViewGl::sizeHint() const
{
	return QSize(400, 300);
}



