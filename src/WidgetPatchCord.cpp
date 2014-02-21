#include "WidgetPatchCord.h"
#include "WidgetInlet.h"
#include "WidgetOutlet.h"
#include "WidgetPatchArea.h"
#include "Outlet.h"
#include <cassert>
#include <QPainter>
#include "Pipeline.h"

using namespace hm;

WidgetPatchCord::WidgetPatchCord(WidgetPatchArea* patchArea)
: QWidget(patchArea)
, mOutlet(nullptr)
, mInlet(nullptr)
, mIsLineDrawn(false)
, mPatchArea(patchArea)
{
    assert(mPatchArea != nullptr);
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
}

WidgetPatchCord::~WidgetPatchCord()
{
	if (!mHasBeenErased)
	{
		hm_error("WidgetPatchCord destroyed before being erased.");
		assert(mHasBeenErased);
	}
}


void WidgetPatchCord::erase()
{
    // This disconnects any patch in the underlying model
    setOutlet(nullptr);
    setInlet(nullptr);
	mPatchArea->deletePatchCord(this);
	mHasBeenErased = true;
}


void WidgetPatchCord::outletDestroyed()
{
    setOutlet(nullptr);
}


void WidgetPatchCord::inletDestroyed()
{
    setInlet(nullptr);
}

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
    
    if (isMouseInteractionActive())
    {
        setMouseTracking(true);
    }
    else
    {
        setMouseTracking(false);
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


void WidgetPatchCord::mousePressEvent(QMouseEvent* event)
{
    if (isMouseInteractionActive())
    {
        if (mInlet==nullptr)
        {
            assert(mOutlet != nullptr);
            WidgetInlet* inlet = mPatchArea->findInlet(event->pos());
            if (inlet && mPatchArea->isConnectionValid(mOutlet, inlet))
            {
                setInlet(inlet);
            }
        }
        else if (mOutlet==nullptr)
        {
            assert(mInlet != nullptr);
            WidgetOutlet* outlet = mPatchArea->findOutlet(event->pos());
            if (outlet && mPatchArea->isConnectionValid(outlet, mInlet))
            {
                setOutlet(outlet);
            }
        }
    }
}

void WidgetPatchCord::mouseMoveEvent(QMouseEvent* event)
{
    if (isMouseInteractionActive())
    {
        if (mOutlet==nullptr)
        {
            assert(mInlet!=nullptr);
            mLine.setP1(event->pos());
        }
        else
        {
            assert(mOutlet!=nullptr && mInlet==nullptr);
            mLine.setP2(event->pos());
        }
    }
}


void WidgetPatchCord::connectOutletSignals()
{
    assert(mOutlet);
    if (mOutlet)
    {
        bool success(true);
        
        success = connect(mOutlet, SIGNAL(newPosition()), this, SLOT(redraw()));
        assert(success);
        
        success = connect(mOutlet, SIGNAL(destroyed()), this, SLOT(outletDestroyed()));
        assert(success);
    }
}

void WidgetPatchCord::connectInletSignals()
{
    assert(mInlet);
    if (mInlet)
    {
        bool success(true);
        
        success = connect(mInlet, SIGNAL(newPosition()), this, SLOT(redraw()));
        assert(success);
        
        success = connect(mInlet, SIGNAL(destroyed()), this, SLOT(inletDestroyed()));
        assert(success);
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
        
        success = disconnect(mOutlet, SIGNAL(destroyed()), this, SLOT(outletDestroyed()));
        assert(success);
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
        
        success = disconnect(mInlet, SIGNAL(destroyed()), this, SLOT(inletDestroyed()));
        assert(success);
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
}

void WidgetPatchCord::connectUnderlyingModel()
{
	assert(mInlet);
    assert(mOutlet);
	OutletPtr outlet = mOutlet->outlet();
	InletPtr inlet = mInlet->inlet();
	assert(!mPatchArea->pipeline()->isConnected(outlet, inlet));
    mPatchArea->pipeline()->connect(outlet, inlet);
   
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


bool WidgetPatchCord::isMouseInteractionActive() const
{
    // mouse interaction is active iff we have only one of (inlet, outlet)
    return (mInlet==nullptr) != (mOutlet==nullptr);
}
