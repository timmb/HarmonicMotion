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

WidgetRenderView::~WidgetRenderView()
{
	hm_debug("WidgetRenderView::~WidgetRenderView. this: "<<this<<" parent: "<<parent());
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

WidgetRenderViewGl::~WidgetRenderViewGl()
{
	hm_debug("~WidgetRenderViewGl::~~WidgetRenderViewGl. this: "<<this<<" parent: "<<parent());
	mTimer->stop();
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
	return QSize(400, 250);
}



