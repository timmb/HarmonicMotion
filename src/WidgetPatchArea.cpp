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
#include <QApplication>
#include "MouseGrabber.h"
#include "WidgetNewPatchCord.h"
#include "PatchAreaMousePressFilter.h"
#include "PatchCord.h"
#include "nodes/NodeRenderer.h"
#include "MetaTypeDeclarations.h"

using namespace hm;

WidgetPatchArea::WidgetPatchArea(PipelinePtr pipeline, QWidget* parent)
: QWidget(parent)
, mPipeline(pipeline)
, mPipelineListener(new PipelineListener(this))
, mNewPatchCord(nullptr)
, mMouseGrabber(new MouseGrabber(this))
, mMousePressFilter(new PatchAreaMousePressFilter(this))
{
	setObjectName("WidgetPatchArea");
	loadStyleSheet();
	if (pipeline==nullptr)
	{
		mPipeline = PipelinePtr(new Pipeline);
	}
	mPipeline->dispatcher->addListener(mPipelineListener);
	// We use queued connections to ensure that the order of
	// events is preserved even though some signals are
	// triggered from different events.
	registerMetaTypes();
	
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigNodeAdded(NodePtr)), this, SLOT(nodeAdded(NodePtr)), Qt::QueuedConnection));
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigNodeRemoved(NodePtr)), this, SLOT(nodeRemoved(NodePtr)), Qt::QueuedConnection));
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigNodeParamsChanged(NodePtr)), this, SLOT(nodeParamsChanged(NodePtr)), Qt::QueuedConnection));
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigNodeCharacteristicsChanged(NodePtr)), this, SLOT(nodeCharacteristicsChanged(NodePtr)), Qt::QueuedConnection));
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigPatchCordAdded(OutletPtr, InletPtr)), this, SLOT(patchCordAdded(OutletPtr, InletPtr)), Qt::QueuedConnection));
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigPatchCordRemoved(OutletPtr, InletPtr)), this, SLOT(patchCordRemoved(OutletPtr, InletPtr)), Qt::QueuedConnection));
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigLoadFromJsonComplete(QStringList)), this, SLOT(loadFromJsonComplete(QStringList)), Qt::QueuedConnection));
	
	// Make changes to pipeline mark this view as dirty
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigNodeAdded(NodePtr)), this, SLOT(markDirty())));
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigNodeRemoved(NodePtr)), this, SLOT(markDirty())));
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigNodeParamsChanged(NodePtr)), this, SLOT(markDirty())));
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigNodeCharacteristicsChanged(NodePtr)), this, SLOT(markDirty())));
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigPatchCordAdded(OutletPtr, InletPtr)), this, SLOT(markDirty())));
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigPatchCordRemoved(OutletPtr, InletPtr)), this, SLOT(markDirty())));
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigParameterChangedInternally(ParameterPtr)), this, SLOT(markDirty())));
	BOOST_VERIFY(connect(mPipelineListener, SIGNAL(sigParameterChangedExternally(ParameterPtr)), this, SLOT(markDirty())));
	
	// Queued connection to allow it to happen on the next cycle of the event
	// loop
	BOOST_VERIFY(connect(this, SIGNAL(sig_resetView()), this, SLOT(slot_resetView()), Qt::QueuedConnection));
	
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setFocusPolicy(Qt::ClickFocus);
	setAutoFillBackground(true);
	installEventFilter(mMousePressFilter);
}

WidgetPatchArea::~WidgetPatchArea()
{
	mPipeline->dispatcher->removeListener(mPipelineListener);
}

QSize WidgetPatchArea::sizeHint() const
{
	return QSize(500, 300);
}

//WidgetNode* WidgetPatchArea::addNode(NodePtr node)
//{
//    WidgetNode* w = new WidgetNode(node, this);
//	mWidgetNodes << w;
//	for (WidgetOutlet* outlet: w->outlets())
//	{
//		mWidgetOutlets << outlet;
//	}
//	for (WidgetInlet* inlet: w->inlets())
//	{
//		mWidgetInlets << inlet;
//	}
//	w->installEventFilter(mMousePressFilter);
//	for (QWidget* child: w->findChildren<QWidget*>())
//	{
//		child->installEventFilter(mMousePressFilter);
//	}
//	w->show();
//    return w;
//}
//
//void WidgetPatchArea::eraseNode(WidgetNode* node)
//{
//	// Remove our weak reference to the node's inlets and outlets.
//	for (WidgetOutlet* outlet: node->outlets())
//	{
//		assert(mWidgetOutlets.count(outlet)==1);
//		mWidgetOutlets.removeOne(outlet);
//	}
//	for (WidgetInlet* inlet: node->inlets())
//	{
//		assert(mWidgetInlets.count(inlet)==1);
//		mWidgetInlets.removeOne(inlet);
//	}
//	assert(mWidgetNodes.count(node)==1);
//	mWidgetNodes.removeOne(node);
//	delete node;
//}

//bool WidgetPatchArea::eraseNodeWithFocus()
//{
//	if (WidgetNode* node = dynamic_cast<WidgetNode*>(QApplication::focusWidget()))
//	{
//		eraseNode(node);
//		return true;
//	}
//	return false;
//}

void WidgetPatchArea::provideInfoPanelText(QString string)
{
	Q_EMIT newInfoPanelText(string);
}

void WidgetPatchArea::loadStyleSheet()
{
	//	QFile file(":/qss/WidgetNode.qss");
	QFile file("/Users/timmb/Documents/Programming/HarmonicMotion/Gui/resources/qss/WidgetPatchArea.qss");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		setStyleSheet(QString::fromUtf8(file.readAll()));
	}
	else
	{
		hm_error("Failed to load stylesheet WidgetPatchArea.qss");
	}
}


//WidgetPatchCord* WidgetPatchArea::createPatchCord(WidgetOutlet* outlet)
//{
//	assert(outlet != nullptr);
//    mNewPatchCord = new WidgetPatchCord(this);
//	mNewPatchCord->setOutlet(outlet);
//	mNewPatchCord->setUnconnectedPointDrawPosition(outlet->connectionPoint());
////	setMouseTracking(true);
//	mMouseGrabber->setEnabled(true);
//	mWidgetPatchCords << mNewPatchCord;
//    return mNewPatchCord;
//}
//
//WidgetPatchCord* WidgetPatchArea::createPatchCord(WidgetInlet* inlet)
//{
//	assert(inlet != nullptr);
//    mNewPatchCord = new WidgetPatchCord(this);
//	mNewPatchCord->setInlet(inlet);
////	setMouseTracking(true);
//	mMouseGrabber->setEnabled(true);
//	mWidgetPatchCords << mNewPatchCord;
//    return mNewPatchCord;
//}
//
//void WidgetPatchArea::endCreationOfPatchCord()
//{
//	mNewPatchCord = nullptr;
////	setMouseTracking(false);
//	mMouseGrabber->setEnabled(false);
//}

//WidgetPatchCord* WidgetPatchArea::addPatchCord(WidgetOutlet* outlet, WidgetInlet* inlet)
//{
//    assert(mWidgetOutlets.contains(outlet));
//    assert(mWidgetInlets.contains(inlet));
//	assert(isConnectionValid(outlet, inlet));
//    WidgetPatchCord* p = new WidgetPatchCord(this, outlet, inlet);
//	mWidgetPatchCords << p;
//	return p;
//}

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

//void WidgetPatchArea::erasePatchCord(WidgetPatchCord* patchCord)
//{
//	assert(mWidgetPatchCords.count(patchCord)==1);
//	mWidgetPatchCords.removeOne(patchCord);
//	patchCord->deleteLater();
//}


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

//WidgetLet* WidgetPatchArea::findLet(QPoint position) const
//{
//	QList<WidgetLet*> lets = findChildren<WidgetLet*>(QRegularExpression("WidgetOutlet|WidgetInlet"));
//	int n = lets.size();
//	for (int i=lets.size()-1; i>=0; i--)
//	{
//		WidgetLet* let = lets[i];
//		if (let->rect().contains(let->mapFromGlobal(mapToGlobal(position))))
//		{
//			return let;
//		}
//	}
//	return nullptr;
//	
//}

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
    && (getPatchCord(outlet, inlet) == nullptr);
	// (no longer enforced:)
//	// check there is at least one datatype from the outlet acceptable to the
//	// inlet
//	&& (outlet->outlet()->types() & inlet->inlet()->types());
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
	qDebug() << "position"<<position;
	if (mNewPatchCord != nullptr)
	{
		// if we're already creating a new patch cord then that should have
		// receied the mouse event. But on rare conditions this doesn't happen
		delete mNewPatchCord;
		mNewPatchCord = nullptr;
	}
	if (let->objectName() == "WidgetOutlet")
	{
		WidgetOutlet* outlet = dynamic_cast<WidgetOutlet*>(let);
		assert(outlet);
		if (outlet)
		{
			mNewPatchCord = new WidgetNewPatchCord(outlet, this);
			hm_debug("Started creating patchcord from "+outlet->outlet()->toString());
		}
	}
	else if (let->objectName() == "WidgetInlet")
	{
		WidgetInlet* inlet = dynamic_cast<WidgetInlet*>(let);
		assert(inlet);
		if (inlet)
		{
			mNewPatchCord = new WidgetNewPatchCord(inlet, this);
			hm_debug("Started creating patchcord from "+inlet->inlet()->toString());
		}
	}
	else
	{
		// We have been passed a bogus WidgetLet
		assert(let->objectName()=="WidgetInlet" || let->objectName()=="WidgetOutlet");
	}
}


void WidgetPatchArea::clearNewPatchCord(WidgetNewPatchCord* toBeCleared)
{
	delete mNewPatchCord;
	mNewPatchCord = nullptr;
}


//void WidgetPatchArea::mouseMoveEvent(QMouseEvent* event)
//{
//	// Check if we're currently creating a new patch cord
//	if (mNewPatchCord != nullptr)
//	{
//		// temp
//		Q_EMIT newInfoPanelText(QString("Mouse %1, %2").arg(event->pos().x()).arg(event->pos().y()));
//		mNewPatchCord->setUnconnectedPointDrawPosition(event->pos());
//	}
//}


//void WidgetPatchArea::keyPressEvent(QKeyEvent* event)
//{
//	if (event->key() == Qt::Key_Escape)
//	{
//		if (mNewPatchCord != nullptr)
//		{
//			mNewPatchCord->erase();
//			mNewPatchCord->deleteLater();
//			mNewPatchCord = nullptr;
//		}
//	}
//}


void WidgetPatchArea::focusInEvent(QFocusEvent* event)
{
	Q_EMIT newInfoPanelText(str(mPipeline->toString()));
}



WidgetNode* WidgetPatchArea::createWidgetFor(NodePtr node)
{
//	if (node->type() == "NodeRenderer")
//	{
//		std::shared_ptr<NodeRenderer> n = std::dynamic_pointer_cast<NodeRenderer>(node);
//		assert(n);
//		if (n)
//		{
//			return new WidgetNodeRenderer(n, this);
//		}
//	}
	return new WidgetNode(node, this);
}



void WidgetPatchArea::nodeAdded(NodePtr node)
{
	// Check to make sure we have a reference to the node. Otherwise,
	// add it.
	for (WidgetNode* w: mWidgetNodes)
	{
		if (w->node() == node)
		{
			// we shouldn't already have a widget if we've received this signal.
			// If we have then things might be out of sync - request a reset.
			assert(false);
			return;
		}
	}
//	addNode(node);
	WidgetNode* w = createWidgetFor(node);
	mWidgetNodes << w;
	for (WidgetOutlet* outlet: w->outlets())
	{
		mWidgetOutlets << outlet;
	}
	for (WidgetInlet* inlet: w->inlets())
	{
		mWidgetInlets << inlet;
	}
	w->installEventFilter(mMousePressFilter);
	for (QWidget* child: w->findChildren<QWidget*>())
	{
		child->installEventFilter(mMousePressFilter);
	}
	w->show();

	// custom node actions
	if (node->type() == "NodeRenderer")
	{
		std::shared_ptr<NodeRenderer> n = std::dynamic_pointer_cast<NodeRenderer>(node);
		assert(n);
		if (n)
		{
			Q_EMIT nodeRendererAdded(n);
		}
	}

}

void WidgetPatchArea::nodeRemoved(NodePtr node)
{
	hm_debug("WidgetPatchArea::nodeRemoved");
	// check there are no patch cords connected to this node (there
	// shouldn't be)
	for (auto it=mWidgetPatchCords.begin(); it!=mWidgetPatchCords.end(); )
	{
		WidgetOutlet* outlet =(**it).outlet();
		WidgetInlet* inlet = (**it).inlet();
		if (inlet->node()->node() == node
			|| outlet->node()->node() == node)
		{
			// nodeRemoved signal received from pipeline while patch cords still attached to node. Something's out of sync, so request a reset.
			resetView();
			if (mPipeline->isConnected(outlet->outlet(), inlet->inlet()))
			{
				mPipeline->disconnect(outlet->outlet(), inlet->inlet());
			}
			it = mWidgetPatchCords.erase(it);
		}
		else
		{
			++it;
		}
	}
	
	// If we have a reference to this node then we need to remove it.
	int size = mWidgetNodes.size();
	auto it=mWidgetNodes.begin();
	for ( ; it!=mWidgetNodes.end(); ++it)
	{
		WidgetNode* w = *it;
		if (w->node() == node)
		{
			// Remove our weak reference to the node's inlets and outlets.
			for (WidgetOutlet* outlet: w->outlets())
			{
				assert(mWidgetOutlets.count(outlet)==1);
				mWidgetOutlets.removeOne(outlet);
			}
			for (WidgetInlet* inlet: w->inlets())
			{
				assert(mWidgetInlets.count(inlet)==1);
				mWidgetInlets.removeOne(inlet);
			}
			assert(mWidgetNodes.count(w)==1);
			mWidgetNodes.removeOne(w);
			delete w;

//			delete *it;
//			mWidgetNodes.erase(it);
			// removes w from mWidgetNodes
//			w->eraseWithoutUpdatingModel();
//			delete w;
//			// Although eraseNode does request that WidgetNode is later deleted
//			// we delete it right away here to ensure we don't retain any shared
//			// pointers to objects in the underlying model longer than necessary
//			assert(!mWidgetNodes.contains(w));
//			delete w;
			break;
		}
	}
	// check we have actually removed a widget
	assert(mWidgetNodes.size() < size);
	// assert that we don't still have a widget for `node`.
	assert(std::all_of(mWidgetNodes.begin(), mWidgetNodes.end(), [&](WidgetNode* w) {
		return w->node() != node;
	}));
	
	// custom node actions
	if (node->type() == "NodeRenderer")
	{
		std::shared_ptr<NodeRenderer> n = std::dynamic_pointer_cast<NodeRenderer>(node);
		assert(n);
		if (n)
		{
			Q_EMIT nodeRendererRemoved(n);
		}
	}
}

void WidgetPatchArea::nodeParamsChanged(NodePtr node)
{
	if (WidgetNode* w = findWidgetFor(node))
	{
		w->updateFromNodeParams();
	}
}

void WidgetPatchArea::nodeCharacteristicsChanged(NodePtr node)
{
	// we destroy and recreate the node widget. Note that we are guaranteed
	// to have no patch cords attached to the node at this point.
	
//	//Note that when we are
//	// in this callback, any obsolete patch cords will already have been
//	// removed, and there can't yet be any new patch cords attached to any
//	// new inlets/outlets. So we just need to destroy and recreate patch
//	// cords that are currently attached. Recreating them we can do by querying
//	// the pipeline.
//	int numPatchCordsDeleted = 0; // for debugging
//	for (auto it=mWidgetPatchCords.begin(); it!=mWidgetPatchCords.end(); )
//	{
//		WidgetPatchCord* p = *it;
//		assert(p != nullptr);
//		InletPtr inlet = p->inlet()->inlet();
//		for (InletPtr i: node->inlets())
//		{
//			if (inlet == i)
//			{
//				delete p;
//				p = nullptr;
//				numPatchCordsDeleted++;
//				it = mWidgetPatchCords.erase(it);
//				break;
//			}
//		}
//		if (p!=nullptr)
//		{
//			OutletPtr outlet = p->outlet()->outlet();
//			for (OutletPtr o: node->outlets())
//			{
//				if (outlet == o)
//				{
//					delete p;
//					p = nullptr;
//					numPatchCordsDeleted++;
//					it = mWidgetPatchCords.erase(it);
//					break;
//				}
//			}
//		}
//		if (p!=nullptr)
//		{
//			// p was not deleted
//			++it;
//		}
//	}
	
	// now delete the widget node. First find it to remember if it has
	// focus
	WidgetNode* w = findWidgetFor(node);
	if (w == nullptr)
	{
		// something has gone wrong
		hm_error("Node characteristics changed on a node with no corresponding"
				 " WidgetNode.");
		assert(false);
		return;
	}
	bool hadFocus = w->hasFocus();
	w = nullptr;
	
	// now destroy and recreate the node widget
	nodeRemoved(node);
	nodeAdded(node);

	w = findWidgetFor(node);
	assert(w != nullptr);
	if (hadFocus)
	{
		w->setFocus();
	}
	
//	// now recreate patchcords
//	int numPatchCordsCreated = 0;
//	std::vector<PatchCordPtr> patchCords = mPipeline->patchCords();
//	for (PatchCordPtr p: patchCords)
//	{
//		// if we don't have a widget for p...
//		if (std::find_if(mWidgetPatchCords.begin(), mWidgetPatchCords.end(), [&](WidgetPatchCord* w) {
//			return w->inlet()->inlet() == p->inlet()
//			&& w->outlet()->outlet() == p->outlet();
//		}) == mWidgetPatchCords.end())
//		{
//			patchCordAdded(p->outlet(), p->inlet());
//			numPatchCordsCreated++;
//		}
//	}
//	assert(numPatchCordsDeleted == numPatchCordsCreated);
}

void WidgetPatchArea::patchCordAdded(OutletPtr outlet, InletPtr inlet)
{
	// If we don't already have a reference to this connection then
	// we need to create a new widget for it
	for (WidgetPatchCord* w: mWidgetPatchCords)
	{
		if (w->outlet()->outlet()==outlet && w->inlet()->inlet()==inlet)
		{
			assert(false);
			return;
		}
	}
	
	auto itWidgetInlet = std::find_if(mWidgetInlets.begin(), mWidgetInlets.end(), [&](WidgetInlet* w) {
		return w->inlet()==inlet;
	});
	if (itWidgetInlet==mWidgetInlets.end())
	{
		// The view has become out of sync with the pipeline. Bail and request
		// a full reset
		resetView();
//		hm_error("Patch cord added to inlet that is not recognised by WidgetPatchArea: "+inlet->toString()+". This is a bug.");
//		qDebug() << "Currently registered nodes:";
//		for (WidgetNode* w: mWidgetNodes)
//		{
//			qDebug() << '-' << str(w->node()->toString());
//		}
//		qDebug() << "Number of inlets:"<<mWidgetInlets.size();
//		assert(itWidgetInlet!=mWidgetInlets.end());
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
				
//	addPatchCord(widgetOutlet, widgetInlet);
    assert(mWidgetOutlets.contains(widgetOutlet));
    assert(mWidgetInlets.contains(widgetInlet));
	assert(isConnectionValid(widgetOutlet, widgetInlet));
    WidgetPatchCord* p = new WidgetPatchCord(this, widgetOutlet, widgetInlet);
	mWidgetPatchCords << p;

}

void WidgetPatchArea::patchCordRemoved(OutletPtr outlet, InletPtr inlet)
{
	hm_debug("WidgetPatchArea::patchCordRemoved");
	// If we have reference to the patch cord then we need to remove it
	int size=mWidgetPatchCords.size();
	
	for (auto it = mWidgetPatchCords.begin() ; it!=mWidgetPatchCords.end(); ++it)
	{
		WidgetPatchCord* w = *it;
		if (w->outlet()->outlet()==outlet && w->inlet()->inlet()==inlet)
		{
			delete *it;
			mWidgetPatchCords.erase(it);
			break;
		}
	}
	// If we did not have a widget to erase then request a reset as things
	// may have gotten out of sync.
	if (mWidgetPatchCords.size() < size)
	{
		resetView();
	}
	// assert we have no more references to this connection
	assert(std::all_of(mWidgetPatchCords.begin(), mWidgetPatchCords.end(), [&](WidgetPatchCord* w) {
		return w->outlet()->outlet()!=outlet || w->inlet()->inlet() != inlet;
	}));
}


void WidgetPatchArea::loadFromJsonComplete(QStringList errors)
{
	// Alerting and marking clean is done by MainWindow
}



void WidgetPatchArea::resizeEvent(QResizeEvent* event)
{
	if (mNewPatchCord != nullptr)
	{
		mNewPatchCord->resize(size());
	}
}





bool WidgetPatchArea::checkToInterceptMousePress(QPoint const& pos, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers)
{
	for (WidgetPatchCord* p: mWidgetPatchCords)
	{
		QPoint posLocalToP = p->mapFrom(this, pos);
		if (p->isLineNear(posLocalToP))
		{
			QMouseEvent event(QEvent::MouseButtonPress, posLocalToP, button, buttons, modifiers);
			p->myMousePressEvent(event);
			return true;
		}
	}
	return false;
}


bool WidgetPatchArea::datatypeInvariant() const
{
	bool invar = true;
	auto nodes = mPipeline->nodes();
	for (NodePtr const node: nodes)
	{
		int i=0;
		for ( ; i<mWidgetNodes.size(); i++)
		{
			if (mWidgetNodes[i]->node() == node)
			{
				break;
			}
		}
		if (i==mWidgetNodes.size())
		{
			invar = false;
			hm_warning("Datatype invariant failed because node was missing widget.");
		}
	}
	for (WidgetNode const* widgetNode: mWidgetNodes)
	{
		int i=0;
		for ( ; i<nodes.size(); i++)
		{
			if (widgetNode->node() == nodes[i])
			{
				break;
			}
		}
		if (i==nodes.size())
		{
			invar = false;
			hm_warning("Datatype invariant failed because widget exists for non-existant node.");
		}
	}
	auto cords = mPipeline->patchCords();
	for (PatchCordPtr const cord: cords)
	{
		int i=0;
		for ( ; i<mWidgetPatchCords.size(); i++)
		{
			WidgetPatchCord const* w = mWidgetPatchCords[i];
			if (cord->inlet()==w->inlet()->inlet()
				&& cord->outlet() == w->outlet()->outlet())
			{
				break;
			}
			if (i==cords.size())
			{
				invar = false;
				hm_warning("Datatype invariant failed because patchcord "
						   "exists without corresponding widget.");
			}
		}
	}
	for (WidgetPatchCord const* w: mWidgetPatchCords)
	{
		int i=0;
		for ( ; i<cords.size(); i++)
		{
			PatchCordPtr cord = cords[i];
			if (cord->inlet()==w->inlet()->inlet()
				&& cord->outlet() == w->outlet()->outlet())
			{
				break;
			}
		}
		if (i==mWidgetPatchCords.size())
		{
			invar = false;
			hm_warning("Datatype invariant failed because widget exists for non-existent patchcord.");
		}
	}
	return invar;
}
			
			
			
void WidgetPatchArea::printWidgets() const
{
	qDebug() << "---\nWidgetNodes:";
	for (WidgetNode* w: mWidgetNodes)
	{
		qDebug() << "  - "<<str(w->node()->name())
		<< w->inlets().size() <<"WidgetInlets,"
		<< w->outlets().size() <<"WidgetOutlets";
	}
	qDebug() << "WidgetPatchcords:";
	for (WidgetPatchCord* w: mWidgetPatchCords)
	{
		qDebug() << " - "<<str(w->outlet()->outlet()->toString())
							  <<" --> "
							  <<str(w->inlet()->inlet()->toString());
	}
}


WidgetNode* WidgetPatchArea::findWidgetFor(NodePtr node) const
{
	auto it = std::find_if(mWidgetNodes.begin(), mWidgetNodes.end(), [&](WidgetNode* w) {
		return w->node() == node;
	});
	if (it == mWidgetNodes.end())
	{
		return nullptr;
	}
	return *it;
	
}


void WidgetPatchArea::raise(WidgetNode* w)
{
	w->raise();
	for (WidgetPatchCord* p: mWidgetPatchCords)
	{
		p->raise();
	}
}


void WidgetPatchArea::markDirty()
{
	mIsDirty = true;
	setWindowModified(true);
}

void WidgetPatchArea::markClean()
{
	mIsDirty = false;
	setWindowModified(false);
}

void WidgetPatchArea::resetView()
{
	// Use a flag to avoid multiple calls to resetView in on cycle of the
	// event loop triggering multiple resets
	mViewNeedsToBeReset = true;
	Q_EMIT sig_resetView();
}

void WidgetPatchArea::slot_resetView()
{
	// currently disabled as it seems to be causing problems.
//	if (mViewNeedsToBeReset)
//	{
//		qDebug() << "View has become out of sync with pipeline. Resetting...";
//		while (!mWidgetPatchCords.empty())
//		{
//			WidgetPatchCord* w = mWidgetPatchCords.back();
//			patchCordRemoved(w->outlet()->outlet(), w->inlet()->inlet());
//		}
//		while (!mWidgetNodes.empty())
//		{
//			nodeRemoved(mWidgetNodes.back()->node());
//		}
//		
//		for (NodePtr node: mPipeline->nodes())
//		{
//			nodeAdded(node);
//		}
//		for (PatchCordPtr p: mPipeline->patchCords())
//		{
//			patchCordAdded(p->outlet(), p->inlet());
//		}
//		mViewNeedsToBeReset = false;
//	}
}


