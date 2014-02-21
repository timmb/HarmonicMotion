#include "WidgetPatchCord.h"
#include "WidgetInlet.h"
#include "WidgetOutlet.h"
#include "WidgetPatchArea.h"
#include "Outlet.h"
#include <cassert>
#include <QPainter>
#include "Pipeline.h"
#include "WidgetNode.h"
#include "Inlet.h"
#include "Outlet.h"
#include <QCoreApplication>
#include "WidgetLet.h"

using namespace hm;

WidgetPatchCord::WidgetPatchCord(WidgetPatchArea* patchArea)
: QWidget(patchArea)
, mOutlet(nullptr)
, mInlet(nullptr)
, mIsLineDrawn(false)
, mPatchArea(patchArea)
{
    assert(mPatchArea != nullptr);
	hm_debug("New disconnected WidgetPatchCord.");
}

WidgetPatchCord::WidgetPatchCord(WidgetPatchArea* patchArea, WidgetOutlet* outlet, WidgetInlet* inlet)
: QWidget(patchArea)
, mOutlet(outlet)
, mInlet(inlet)
, mIsLineDrawn(false)
, mPatchArea(patchArea)
, mHasBeenErased(false)
{
    assert(mPatchArea != nullptr);
    assert(mOutlet != nullptr);
    assert(mOutlet->outlet()->isConnectedTo(mInlet->inlet()));
    assert(mInlet != nullptr);
    connectInletSignals();
    connectOutletSignals();
    redraw();
	hm_debug("New WidgetPatchCord with outlet "+mOutlet->outlet()->toString()+" and inlet "+mInlet->inlet()->toString());
	show();
}

WidgetPatchCord::~WidgetPatchCord()
{
	if (!mHasBeenErased)
	{
		hm_error("WidgetPatchCord destroyed before being erased.");
		assert(mHasBeenErased);
	}
	hm_debug("WidgetPatchCord destroyed.");
}


void WidgetPatchCord::erase()
{
	// NB it is important that the patch area is updated before the
	// underlying model to ensure that callbacks from the model do not
	// cause the patch area to attempt to start removing this widget
	// a second time.
	mPatchArea->erasePatchCord(this);
    // This disconnects any patch in the underlying model
    setOutlet(nullptr);
    setInlet(nullptr);
	mHasBeenErased = true;
}


void WidgetPatchCord::eraseWithoutUpdatingModel()
{
	mPatchArea->erasePatchCord(this);
	mHasBeenErased = true;
}


//void WidgetPatchCord::outletDestroyed()
//{
//    setOutlet(nullptr);
//}
//
//
//void WidgetPatchCord::inletDestroyed()
//{
//    setInlet(nullptr);
//}

void WidgetPatchCord::redraw()
{
    if (mInlet==nullptr && mOutlet==nullptr)
    {
        mIsLineDrawn = false;
    }
    else
    {
        mIsLineDrawn = true;
        if (mOutlet!=nullptr)
        {
            mLine.setP1(mOutlet->connectionPoint());
        }
        if (mInlet!=nullptr)
        {
            mLine.setP2(mInlet->connectionPoint());
        }
        QRect bounds;
        bounds.setLeft(qMin(mLine.x1(), mLine.x2()));
        bounds.setTop(qMin(mLine.y1(), mLine.y2()));
        bounds.setRight(qMax(mLine.x1(), mLine.x2()));
        bounds.setBottom(qMax(mLine.y1(), mLine.y2()));
        setGeometry(bounds);
    }
    update();
}



void WidgetPatchCord::paintEvent(QPaintEvent* event)
{
    if (mIsLineDrawn)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(QColor(50,50,50)));
        painter.drawLine(mLine);
    }
}


bool WidgetPatchCord::trySettingUnconnectedLet(WidgetLet* let)
{
	if (!isPartiallyConnected())
	{
		hm_error("trySettingsUnconnectedPoint called on WidgetPatchCord that is not partially connected,");
		assert(isPartiallyConnected());
		return false;
	}
	if (mInlet==nullptr)
	{
		WidgetInlet* inlet = dynamic_cast<WidgetInlet*>(let);
		if (inlet && (inlet->inlet()->types() & mOutlet->outlet()->types()))
		{
			setInlet(inlet);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		assert(mOutlet==nullptr);
		WidgetOutlet* outlet = dynamic_cast<WidgetOutlet*>(let);
		if (outlet && (mInlet->inlet()->types() & outlet->outlet()->types()))
		{
			setOutlet(outlet);
			return true;
		}
		else
		{
			return false;
		}
	}
}


//void WidgetPatchCord::mousePressEvent(QMouseEvent* event)
//{
//	hm_debug("WidgetPatchCord::mousePressEvent");
//    if (isPartiallyConnected())
//    {
//		event->accept();
//        if (mInlet==nullptr)
//        {
//            assert(mOutlet != nullptr);
//            WidgetInlet* inlet = mPatchArea->findInlet(event->pos());
//			hm_debug("Found inlet");
//            if (inlet && mPatchArea->isConnectionValid(mOutlet, inlet))
//            {
//                setInlet(inlet);
//				hm_debug("WidgetPatchCord inlet set to "+inlet->inlet()->toString());
//            }
//        }
//        else if (mOutlet==nullptr)
//        {
//            assert(mInlet != nullptr);
//            WidgetOutlet* outlet = mPatchArea->findOutlet(event->pos());
//			hm_debug("Found outlet");
//            if (outlet && mPatchArea->isConnectionValid(outlet, mInlet))
//            {
//                setOutlet(outlet);
//				hm_debug("WidgetPatchCord outlet set to "+outlet->outlet()->toString());
//            }
//        }
//    }
//}

//void WidgetPatchCord::mouseMoveEvent(QMouseEvent* event)
//{
////	hm_debug("WidgetPatchCord::mouseMoveEvent");
//    if (isMouseInteractionActive())
//    {
//		event->accept();
//        if (mOutlet==nullptr)
//        {
//            assert(mInlet!=nullptr);
//            mLine.setP1(event->pos());
//        }
//        else
//        {
//            assert(mOutlet!=nullptr && mInlet==nullptr);
//            mLine.setP2(event->pos());
//        }
//    }
//}


//void WidgetPatchCord::mousePressEvent(QMouseEvent* event)
//{
//	QCoreApplication::sendEvent(mPatchArea, event);
//}


void WidgetPatchCord::setUnconnectedPointDrawPosition(QPoint position)
{
	assert((mOutlet==nullptr) != (mInlet==nullptr));
	
	if (mOutlet==nullptr)
	{
		mLine.setP1(position);
	}
	else
	{
		mLine.setP2(position);
	}
}



void WidgetPatchCord::connectOutletSignals()
{
    assert(mOutlet);
    if (mOutlet)
    {
        bool success(true);
        
        success = connect(mOutlet->node(), SIGNAL(geometryChanged()), this, SLOT(redraw()));
        assert(success);
        
//        success = connect(mOutlet, SIGNAL(destroyed()), this, SLOT(outletDestroyed()));
//        assert(success);
    }
}

void WidgetPatchCord::connectInletSignals()
{
    assert(mInlet);
    if (mInlet)
    {
        bool success(true);
        
        success = connect(mInlet->node(), SIGNAL(geometryChanged()), this, SLOT(redraw()));
        assert(success);
        
//        success = connect(mInlet, SIGNAL(destroyed()), this, SLOT(inletDestroyed()));
//        assert(success);
    }
}

void WidgetPatchCord::disconnectOutletSignals()
{
    assert(mInlet);
    
    if (mInlet)
    {
        bool success(true);
        
        success = disconnect(mOutlet, SIGNAL(newPosition()), this, SLOT(outletMoved()));
        assert(success);
        
//        success = disconnect(mOutlet, SIGNAL(destroyed()), this, SLOT(outletDestroyed()));
//        assert(success);
    }
}

void WidgetPatchCord::disconnectInletSignals()
{
    assert(mInlet);
    
    if (mInlet)
    {
        bool success(true);
        
        success = disconnect(mInlet, SIGNAL(newPosition()), this, SLOT(inletMoved()));
        assert(success);
        
//        success = disconnect(mInlet, SIGNAL(destroyed()), this, SLOT(inletDestroyed()));
//        assert(success);
    }
}

void WidgetPatchCord::disconnectUnderlyingModel()
{
	assert(mInlet);
    assert(mOutlet);
	OutletPtr outlet = mOutlet->outlet();
	InletPtr inlet = mInlet->inlet();
	assert(mPatchArea->pipeline()->isConnected(outlet, inlet));
	mPatchArea->pipeline()->disconnect(outlet, inlet);
	hm_info(outlet->toString()+" disconnected via WidgetPatchCord from "+inlet->toString());
}

void WidgetPatchCord::connectUnderlyingModel()
{
	assert(mInlet);
    assert(mOutlet);
	OutletPtr outlet = mOutlet->outlet();
	InletPtr inlet = mInlet->inlet();
	assert(!mPatchArea->pipeline()->isConnected(outlet, inlet));
    mPatchArea->pipeline()->connect(outlet, inlet);
	hm_info(outlet->toString()+" connected via WidgetPatchCord to "+inlet->toString());
}

void WidgetPatchCord::setOutlet(WidgetOutlet* outlet)
{
    if (mOutlet)
    {
        disconnectOutletSignals();
        if (mInlet)
        {
            disconnectUnderlyingModel();
        }
    }
    mOutlet = outlet;
	if (mOutlet)
	{
		hm_debug("WidgetPatchCord: outlet set to "+mOutlet->outlet()->toString());
	}
	else
	{
		hm_debug("WidgetPatchCord: outlet set to nullptr");
	}
    if (mOutlet)
    {
        connectOutletSignals();
        // connect on the underlying model
        if (mInlet)
        {
            connectUnderlyingModel();
        }
    }
    redraw();
}

void WidgetPatchCord::setInlet(WidgetInlet* inlet)
{
    if (mInlet)
    {
        disconnectInletSignals();
        // if mOutlet is defined then this instance represents an existing
        // connection
        if (mOutlet)
        {
            disconnectUnderlyingModel();
        }
    }
    mInlet = inlet;
    if (mInlet)
    {
        connectInletSignals();
        if (mOutlet)
        {
            connectUnderlyingModel();
        }
    }
    redraw();
}


bool WidgetPatchCord::isPartiallyConnected() const
{
    // mouse interaction is active iff we have only one of (inlet, outlet)
    return (mInlet==nullptr) != (mOutlet==nullptr);
}


bool WidgetPatchCord::isFullyConnected() const
{
	return mInlet!=nullptr && mOutlet!=nullptr;
}


