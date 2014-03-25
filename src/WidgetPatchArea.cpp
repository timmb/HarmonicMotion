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
#include <QCoreApplication>
#include <QRegularExpression>
#include "Utilities.h"

using namespace hm;

WidgetPatchArea::WidgetPatchArea(PipelinePtr pipeline, QWidget* parent)
: QWidget(parent)
, mPipeline(pipeline)
, mNewPatchCord(nullptr)
{
	if (pipeline==nullptr)
	{
		mPipeline = PipelinePtr(new Pipeline);
	}
	mPipeline->addListener(this);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setFocusPolicy(Qt::ClickFocus);
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
	mWidgetNodes << w;
	for (WidgetOutlet* outlet: w->outlets())
	{
		mWidgetOutlets << outlet;
	}
	for (WidgetInlet* inlet: w->inlets())
	{
		mWidgetInlets << inlet;
	}
    return w;
}

void WidgetPatchArea::eraseNode(WidgetNode* node)
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

void WidgetPatchArea::provideInfoPanelText(QString string)
{
	Q_EMIT newInfoPanelText(string);
}

WidgetPatchCord* WidgetPatchArea::createPatchCord(WidgetOutlet* outlet)
{
	assert(outlet != nullptr);
    WidgetPatchCord* p = new WidgetPatchCord(this);
	p->setOutlet(outlet);
	p->setUnconnectedPointDrawPosition(outlet->connectionPoint());
	mNewPatchCord = p;
	setMouseTracking(true);
    return p;
}

WidgetPatchCord* WidgetPatchArea::createPatchCord(WidgetInlet* inlet)
{
	assert(inlet != nullptr);
    WidgetPatchCord* p = new WidgetPatchCord(this);
	p->setInlet(inlet);
	mNewPatchCord = p;
	setMouseTracking(true);
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

void WidgetPatchArea::erasePatchCord(WidgetPatchCord* patchCord)
{
	assert(mWidgetPatchCords.count(patchCord)==1);
	mWidgetPatchCords.removeOne(patchCord);
	patchCord->deleteLater();
}


WidgetOutlet* WidgetPatchArea::findOutlet(QPoint position) const
{
	QList<WidgetOutlet*> outlets = findChildren<WidgetOutlet*>("WidgetOutlet");
	for (int i=outlets.size()-1; i>=0; i--)
	{
		WidgetOutlet* outlet = outlets[i];
		// check if position is inside outlet geometry
		if (outlet->rect().contains(outlet->mapFromGlobal(mapToGlobal(position))))
		{
			return outlet;
		}
	}
	return nullptr;
}

WidgetInlet* WidgetPatchArea::findInlet(QPoint position) const
{
	QList<WidgetInlet*> inlets = findChildren<WidgetInlet*>("WidgetInlet");
	for (int i=inlets.size()-1; i>=0; i--)
	{
		WidgetInlet* inlet = inlets[i];
		if (inlet->rect().contains(inlet->mapFromGlobal(mapToGlobal(position))))
		{
			return inlet;
		}
	}
	return nullptr;
}

WidgetLet* WidgetPatchArea::findLet(QPoint position) const
{
	QList<WidgetLet*> lets = findChildren<WidgetLet*>(QRegularExpression("WidgetOutlet|WidgetInlet"));
	int n = lets.size();
	for (int i=lets.size()-1; i>=0; i--)
	{
		WidgetLet* let = lets[i];
		if (let->rect().contains(let->mapFromGlobal(mapToGlobal(position))))
		{
			return let;
		}
	}
	return nullptr;
	
}

//WidgetOutlet* WidgetPatchArea::findOutlet(QPoint position) const
//{
//	QList<WidgetOutlet*> outlets = findChildren<WidgetOutlet*>();
//	// later elements in findChild() are on top of earlier elements
//	// so search backwards
//	std::reverse_iterator<QList<WidgetOutlet*>::iterator> it(outlets.begin()), it_end(outlets.end());
//	for ( ; it!=it_end; ++it)
//	{
//		if ((**it).underMouse())
//		{
//			return *it;
//		}
//	}
//	return nullptr;
//}
//
//WidgetInlet* WidgetPatchArea::findInlet(QPoint position) const
//{
//    return dynamic_cast<WidgetInlet*>(childAt(position));
//}
//
//WidgetLet* WidgetPatchArea::findLet(QPoint position) const
//{
//	return dynamic_cast<WidgetLet*>(childAt(position));
//}

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
//	qDebug() << "settings patcharea geometry to"<<content;
	setGeometry(content);
}

void WidgetPatchArea::mousePressEventFromWidgetLet(WidgetLet* let, QPoint position)
{
	hm_debug("mousePressEventFromWidgetLet::mousePressEvent");
	// if we're currently creating a patch cord then forward it the event
	if (mNewPatchCord != nullptr)
	{
		hm_debug("mouse press event received during creation of new patchcord");
		if (let)
		{
			mNewPatchCord->trySettingUnconnectedLet(let);
		}
		// if we've completed the patch cord, move to the normal collection
		if (mNewPatchCord->isFullyConnected())
		{
			hm_debug("New patchcord successfully created");
			mWidgetPatchCords << mNewPatchCord;
			mNewPatchCord = nullptr;
			setMouseTracking(false);
		}
	}
	// otherwise if an inlet or outlet has been pressed then we start
	// creating a new patch cord
	else
	{
		if (let->objectName() == "WidgetOutlet")
		{
			WidgetOutlet* outlet = dynamic_cast<WidgetOutlet*>(let);
			assert(outlet);
			if (outlet)
			{
				createPatchCord(outlet);
			hm_debug("Started creating patchcord from "+outlet->outlet()->toString());
			}
		}
		else if (let->objectName() == "WidgetInlet")
		{
			WidgetInlet* inlet = dynamic_cast<WidgetInlet*>(let);
			assert(inlet);
			if (inlet)
			{
				createPatchCord(inlet);
				hm_debug("Started creating patchcord from "+inlet->inlet()->toString());
			}
		}
		else
		{
			// We have been passed a bogus WidgetLet
			assert(let->objectName()=="WidgetInlet" || let->objectName()=="WidgetOutlet");
		}
		setMouseTracking(true);
	}
	
	
	
//	// Check if we're currently creating a new patch cord
//	if (mNewPatchCord != nullptr)
//	{
//		// if we are currently editing a patch cord then it should have
//		// either no inlet xor no outlet.
//		assert((mNewPatchCord->inlet()==nullptr) != (mNewPatchCord->outlet()==nullptr));
//		if (mNewPatchCord->inlet() == nullptr)
//		{
//			WidgetInlet* inlet = findInlet(event->pos());
//			if (inlet)
//			{
//				mNewPatchCord->setInlet(inlet);
//			}
//		}
//		else
//		{
//			WidgetOutlet* outlet = findOutlet(event->pos());
//			if (outlet)
//			{
//				mNewPatchCord->setOutlet(outlet);
//			}
//		}
//		// if we've completed the patch cord, move to the normal collection
//		if (mNewPatchCord->inlet()!=nullptr && mNewPatchCord->outlet()!=nullptr)
//		{
//			mWidgetPatchCords << mNewPatchCord;
//			mNewPatchCord = nullptr;
//		}
//	}
}


void WidgetPatchArea::mouseMoveEvent(QMouseEvent* event)
{
	// Check if we're currently creating a new patch cord
	if (mNewPatchCord != nullptr)
	{
		mNewPatchCord->setUnconnectedPointDrawPosition(event->pos());
	}
}


void WidgetPatchArea::focusInEvent(QFocusEvent* event)
{
	Q_EMIT newInfoPanelText(str(mPipeline->toString()));
}




void WidgetPatchArea::nodeAdded(NodePtr node)
{
	// Check to make sure we have a reference to the node. Otherwise,
	// add it.
	for (WidgetNode* w: mWidgetNodes)
	{
		if (w->node() == node)
		{
			return;
		}
	}
	addNode(node);
}

void WidgetPatchArea::nodeRemoved(NodePtr node)
{
	// If we have a reference to this node then we need to remove it
	// without attempting to re-remove it from the pipeline.
	for (WidgetNode* w: mWidgetNodes)
	{
		if (w->node() == node)
		{
			w->eraseWithoutUpdatingModel();
			break;
		}
	}
	// assert that we don't still have a widget for `node`.
	assert(std::all_of(mWidgetNodes.begin(), mWidgetNodes.end(), [&](WidgetNode* w) {
		return w->node() != node;
	}));
}

void WidgetPatchArea::patchCordAdded(OutletPtr outlet, InletPtr inlet)
{
	// If we don't already have a reference to this connection then
	// we need to create a new widget for it
	for (WidgetPatchCord* w: mWidgetPatchCords)
	{
		if (w->outlet()->outlet()==outlet && w->inlet()->inlet()==inlet)
		{
			return;
		}
	}
	
	auto itWidgetInlet = std::find_if(mWidgetInlets.begin(), mWidgetInlets.end(), [&](WidgetInlet* w) {
		return w->inlet()==inlet;
	});
	if (itWidgetInlet==mWidgetInlets.end())
	{
		hm_error("Patch cord added to inlet that is not recognised by WidgetPatchArea: "+inlet->toString()+". This is a bug.");
		qDebug() << "Currently registered nodes:";
		for (WidgetNode* w: mWidgetNodes)
		{
			qDebug() << '-' << str(w->node()->toString());
		}
		qDebug() << "Number of inlets:"<<mWidgetInlets.size();
		assert(itWidgetInlet!=mWidgetInlets.end());
			   return;
	}
	WidgetInlet* widgetInlet = *itWidgetInlet;
	
	auto itWidgetOutlet = std::find_if(mWidgetOutlets.begin(), mWidgetOutlets.end(), [&](WidgetOutlet* w) {
		return w->outlet()==outlet;
	});
	if (itWidgetInlet==mWidgetInlets.end())
	{
		hm_error("Patch cord added from outlet that is not recognised by WidgetPatchArea: "+outlet->toString()+". This is a bug.");
		assert(itWidgetOutlet!=mWidgetOutlets.end());
		return;
	}
	WidgetOutlet* widgetOutlet = *itWidgetOutlet;
				
	addPatchCord(widgetOutlet, widgetInlet);
}

void WidgetPatchArea::patchCordRemoved(OutletPtr outlet, InletPtr inlet)
{
	// If we have refernce to the patch cord then we need to remove it
	for (WidgetPatchCord* w: mWidgetPatchCords)
	{
		if (w->outlet()->outlet()==outlet && w->inlet()->inlet()==inlet)
		{
			w->eraseWithoutUpdatingModel();
			break;
		}
	}
	// assert we have no more references to this connection
	assert(std::all_of(mWidgetPatchCords.begin(), mWidgetPatchCords.end(), [&](WidgetPatchCord* w) {
		return w->outlet()->outlet()!=outlet && w->inlet()->inlet() != inlet;
	}));
}





