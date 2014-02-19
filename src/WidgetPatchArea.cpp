//
//  WidgetPatchArea.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 05/02/2014.
//
//

#include "WidgetPatchArea.h"
#include <cassert>
#include <QDebug>
#include "WidgetPatchCord.h"
#include "Inlet.h"
#include "Outlet.h"
#include "WidgetInlet.h"
#include "WidgetOutlet.h"
#include "Node.h"
#include "WidgetNode.h"

using namespace hm;

WidgetPatchArea::WidgetPatchArea(QWidget* parent)
: QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setAutoFillBackground(true);
//	mPatchArea->setColor(QPalette::Window, QColor("gray"));
}

QSize WidgetPatchArea::sizeHint() const
{
	return QSize(500, 300);
}

WidgetNode* WidgetPatchArea::addNode(NodePtr node)
{
    WidgetNode* w = new WidgetNode(node, this);
    connect(w, &QObject::destroyed, [=]() {
        callbackWidgetNodeDestroyed(w);
    });
    return w;
}

void WidgetPatchArea::callbackWidgetNodeDestroyed(WidgetNode* w)
{
    NodePtr node = w->node();
    assert(mWidgetNodes.contains(node));
    mWidgetNodes.remove(node);
    for (WidgetInlet* i: w->widgetInlets())
    {
        InletPtr inlet = i->inlet();
        assert(mWidgetInlets.contains(inlet));
        mWidgetInlets.remove(inlet);
    }
    for (WidgetOutlet* o: w->widgetOutlets())
    {
        OutletPtr outlet = o->outlet();
        assert(mWidgetOutlets.contains(outlet));
        mWidgetOutlets.remove(outlet);
    }
}

WidgetPatchCord* WidgetPatchArea::addPatchCord()
{
    WidgetPatchCord* p = new WidgetPatchCord(this);
    return p;
}

WidgetPatchCord* WidgetPatchArea::addPatchCord(OutletPtr outlet, InletPtr inlet)
{
    assert(mWidgetOutlets.contains(outlet));
    assert(mWidgetInlets.contains(inlet));
    return new WidgetPatchCord(this, mWidgetOutlets.value(outlet), mWidgetInlets.value(inlet));
}

WidgetInlet* WidgetPatchArea::findInlet(QPoint position) const
{
    QWidget* child = childAt(position);
    if (child && child->property("isInlet")==true)
    {
        WidgetInlet* w = dynamic_cast<WidgetInlet*>(child);
        assert(w);
        if (w)
        {
            return w;
        }
    }
    return nullptr;
}

WidgetOutlet* WidgetPatchArea::findOutlet(QPoint position) const
{
    QWidget* child = childAt(position);
    if (child && child->property("isOutlet")==true)
    {
        WidgetOutlet* w = dynamic_cast<WidgetOutlet*>(child);
        assert(w);
        if (w)
        {
            return w;
        }
    }
    return nullptr;
}

void WidgetPatchArea::updateSize()
{
	QRect content = childrenRect();
	content.setLeft(0);
	content.setTop(0);
	content.setWidth(qMax(sizeHint().width(), content.width()));
	content.setHeight(qMax(sizeHint().height(), content.height()));
	qDebug() << "settings patcharea geometry to"<<content;
	setGeometry(content);
}