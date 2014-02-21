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
#include "Pipeline.h"

using namespace hm;

WidgetPatchArea::WidgetPatchArea(PipelinePtr pipeline, QWidget* parent)
: QWidget(parent)
, mPipeline(pipeline)
{
	if (pipeline==nullptr)
	{
		mPipeline = PipelinePtr(new Pipeline);
	}
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
    return w;
}

void WidgetPatchArea::deleteNode(WidgetNode* node)
{
	// Remove our weak reference to the node's inlets and outlets.
	for (WidgetOutlet* outlet: node->outlets())
	{
		assert(mWidgetOutlets.count(outlet)==1);
		mWidgetOutlets.removeOne(outlet);
	}
	for (WidgetInlet* inlet: node->inlets())
	{
		assert(mWidgetInlets.count(inlet)==1);
		mWidgetInlets.removeOne(inlet);
	}
	assert(mWidgetNodes.count(node)==1);
	mWidgetNodes.removeOne(node);
	node->deleteLater();
}

//void WidgetPatchArea::callbackWidgetNodeDestroyed(WidgetNode* w)
//{
//    NodePtr node = w->node();
//    assert(mWidgetNodes.contains(node));
//    mWidgetNodes.remove(node);
//    for (WidgetInlet* i: w->widgetInlets())
//    {
//        InletPtr inlet = i->inlet();
//        assert(mWidgetInlets.contains(inlet));
//        mWidgetInlets.remove(inlet);
//    }
//    for (WidgetOutlet* o: w->widgetOutlets())
//    {
//        OutletPtr outlet = o->outlet();
//        assert(mWidgetOutlets.contains(outlet));
//        mWidgetOutlets.remove(outlet);
//    }
//}

WidgetPatchCord* WidgetPatchArea::createPatchCord(WidgetOutlet* outlet)
{
	assert(outlet != nullptr);
    WidgetPatchCord* p = new WidgetPatchCord(this);
	p->setOutlet(outlet);
	mWidgetPatchCords.push_back(p);
    return p;
}

WidgetPatchCord* WidgetPatchArea::createPatchCord(WidgetInlet* inlet)
{
	assert(inlet != nullptr);
    WidgetPatchCord* p = new WidgetPatchCord(this);
	p->setInlet(inlet);
	mWidgetPatchCords.push_back(p);
    return p;
}

WidgetPatchCord* WidgetPatchArea::addPatchCord(WidgetOutlet* outlet, WidgetInlet* inlet)
{
    assert(mWidgetOutlets.contains(outlet));
    assert(mWidgetInlets.contains(inlet));
	assert(isConnectionValid(outlet, inlet));
    WidgetPatchCord* p = new WidgetPatchCord(this, outlet, inlet);
	mWidgetPatchCords << p;
	return p;
}

WidgetPatchCord* WidgetPatchArea::getPatchCord(WidgetOutlet* outlet, WidgetInlet* inlet) const
{
	for (WidgetPatchCord* w: mWidgetPatchCords)
	{
		if (w->outlet() == outlet && w->inlet() == inlet)
		{
			return w;
		}
	}
	return nullptr;
}

void WidgetPatchArea::deletePatchCord(WidgetPatchCord* patchCord)
{
	assert(mWidgetPatchCords.count(patchCord)==1);
	mWidgetPatchCords.removeOne(patchCord);
	patchCord->deleteLater();
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

bool WidgetPatchArea::isConnectionValid(WidgetOutlet* outlet, WidgetInlet* inlet) const
{
    return (outlet != nullptr)
    && (inlet != nullptr)
	// check connection doesn't already exist
    && (getPatchCord(outlet, inlet) == nullptr)
	// check there is at least one datatype from the outlet acceptable to the
	// inlet
	&& (outlet->outlet()->types() & inlet->inlet()->types());
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