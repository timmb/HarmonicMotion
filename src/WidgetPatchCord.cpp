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
#include "Utilities.h"

#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QAction>
#include <cassert>


using namespace hm;

//WidgetPatchCord::WidgetPatchCord(WidgetPatchArea* patchArea)
//: QWidget(patchArea)
//, mOutlet(nullptr)
//, mInlet(nullptr)
//, mIsLineDrawn(false)
//, mPatchArea(patchArea)
//{
//    assert(mPatchArea != nullptr);
//	hm_debug("New disconnected WidgetPatchCord.");
//	
////	QTimer* t = new QTimer;
////	connect(t, SIGNAL(timeout()), this, SLOT(loadStyleSheet()));
////	t->setInterval(500);
////	t->start();
//}

//namespace {
//
//
//// based on http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
//float distanceFromPointToLine(QVector2D const& lineStart, QVector2D const& lineEnd, QVector2D const& point) {
//	QVector2D const& v = lineStart;
//	QVector2D const& w = lineEnd;
//	QVector2D const& p = point;
//	// Return minimum distance between line segment vw and point p
//	float const l2 = (v - w).lengthSquared();  // i.e. |w-v|^2 -  avoid a sqrt
//	if (l2 == 0.0) return (p-v).length();   // v == w case
//	// Consider the line extending the segment, parameterized as v + t (w - v).
//	// We find projection of point p onto the line.
//	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
//	float const t = QVector2D::dotProduct(p - v, w - v) / l2;
//	if (t < 0.0)
//		return (p - v).length();       // Beyond the 'v' end of the segment
//	else if (t > 1.0)
//		return (p - w).length();  // Beyond the 'w' end of the segment
//	QVector2D const projection = v + t * (w - v);  // Projection falls on the segment
//	return (p - projection).length();
//}
//
//}
//
//
//WidgetPatchCordMouseFilter::WidgetPatchCordMouseFilter(WidgetPatchCord* parent)
//: QObject(parent)
//, mCord(parent)
//{
//	assert(parent != nullptr);
//}
//
//bool WidgetPatchCordMouseFilter::eventFilter(QObject* object, QEvent* event)
//{
//	assert(object == mCord);
//	if (event->type() == QEvent::MouseButtonPress)
//	{
//		QMouseEvent* e = dynamic_cast<QMouseEvent*>(event);
//		assert(e);
//		if (e)
//		{
//			QPoint mouseRelativeToParent = mCord->mapToParent(e->pos());
//			// calculate distance of mouse click to patch cord
//			QLineF line(mCord->line());
//			float distance = distanceFromPointToLine(QVector2D(line.p1()), QVector2D(line.p2()), QVector2D(mouseRelativeToParent));
//			qDebug() << distance;
//			const float distanceThreshold = 2.f;
//			if (distance < distanceThreshold)
//			{
//				qDebug() << "consuming mouse click for focus";
//				mCord->setFocus(Qt::MouseFocusReason);
//				return true;
//			}
//			else{
//				qDebug() << "mousepressevent too far for focus";
//				return QObject::eventFilter(object, event);
//			}
//		}
//	}
//	return false;
//}



WidgetPatchCord::WidgetPatchCord(WidgetPatchArea* patchArea, WidgetOutlet* outlet, WidgetInlet* inlet)
: QWidget(patchArea)
, mOutlet(outlet)
, mInlet(inlet)
//, mIsLineDrawn(false)
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
	
//	setFocusPolicy(Qt::ClickFocus);
//	setFocusPolicy(Qt::NoFocus);
//	installEventFilter(new WidgetPatchCordMouseFilter(this));
	setAttribute(Qt::WA_TransparentForMouseEvents);
	
	// ACTIONS
	QAction* eraseAction = new QAction("Delete", this);
	eraseAction->setShortcuts(QList<QKeySequence>() << QKeySequence::Delete << Qt::Key_Backspace);
	eraseAction->setShortcutContext(Qt::WidgetShortcut);
	addAction(eraseAction);
	bool success = connect(eraseAction, SIGNAL(triggered()), this, SLOT(erase()));
	assert(success);
	
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

//
//void WidgetPatchCord::loadStyleSheet()
//{
//	QFile file("/Users/timmb/Documents/Programming/HarmonicMotion/HarmonicMotionGui/resources/qss/WidgetPatchCord.qss");
//	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
//	{
//		setStyleSheet(QString::fromUtf8(file.readAll()));
//	}
//	else
//	{
//		hm_error("Failed to load stylesheet WidgetPatchCord.qss");
//	}
//
//}


void WidgetPatchCord::erase()
{
	bool success = mPatchArea->pipeline()->disconnect(mOutlet->outlet(), mInlet->inlet());
	assert(success && "Patchcord is removed from underlying model here.");
	
//	// NB it is important that the patch area is updated before the
//	// underlying model to ensure that callbacks from the model do not
//	// cause the patch area to attempt to start removing this widget
//	// a second time.
//	mPatchArea->erasePatchCord(this);
//    // This disconnects any patch in the underlying model
//    setOutlet(nullptr);
//    setInlet(nullptr);
	mHasBeenErased = true;
}


void WidgetPatchCord::eraseWithoutUpdatingModel()
{
	mPatchArea->erasePatchCord(this);
	mHasBeenErased = true;
}


void WidgetPatchCord::redraw()
{
	assert (mInlet != nullptr && mOutlet != nullptr);
//    if (mInlet==nullptr && mOutlet==nullptr)
//    {
//        mIsLineDrawn = false;
////		qDebug() << "redraw line is not drawn";
//    }
//    else
//    {
//        mIsLineDrawn = true;
//		qDebug() << "redraw line is drawn";
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
	// minimum size to make sure we can click on the wiget
	bounds.setWidth(qMax(bounds.width(), 2));
	bounds.setHeight(qMax(bounds.height(), 2));
        setGeometry(bounds);
//		qDebug() << "line"<<mLine<<"bounds"<<bounds;
//    }
    update();
}



void WidgetPatchCord::paintEvent(QPaintEvent* event)
{
//	qDebug() << "paintEvent";
//    if (mIsLineDrawn)
//    {
//		qDebug() << "line is drawn";
		QLine lineRelativeToThis = mLine.translated(-pos());
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
	if (hasFocus())
	{
		QPen pen(QColor(220, 150, 150));
		pen.setWidth(2.5f);
		painter.setPen(pen);
		painter.drawLine(lineRelativeToThis);
	}
        painter.setPen(QPen(QColor(50,50,50)));
        painter.drawLine(lineRelativeToThis);
//    }
}


void WidgetPatchCord::myMousePressEvent(QMouseEvent const& event)
{
	setFocus(Qt::MouseFocusReason);
}


bool WidgetPatchCord::isLineNear(QPoint const& point) const
{
	QLine relLine(mLine.p1() - pos(), mLine.p2() - pos());
	double d = distanceFromPointToLine(QVector2D(relLine.p1()), QVector2D(relLine.p2()), QVector2D(point));
	return d < 2.f;
}

//void WidgetPatchCord::mousePressEvent(QMouseEvent* event)
//{
////	mMouseClickPosition = event->localPos();
////	qDebug() << "mouse relative pos " << mMouseClickPosition;
////	QPoint mouseRelativeToParent = mapToParent(event->pos());
//////	event->ignore();
////	// check for focus
////	const float distanceThreshold = 2.f;
////	// calculate distance of mouse click to patch cord
////	QLineF line(mLine);
////	float distance = distanceFromPointToLine(QVector2D(mLine.p1()), QVector2D(mLine.p2()), QVector2D(mouseRelativeToParent));
////	qDebug() << distance;
////	if (distance > distanceThreshold)
////	{
////		qDebug() << "ignoring";
////		event->ignore();
////	}
////	else{
////		qDebug() << "mousepressevent focus";
////		setFocus(Qt::MouseFocusReason);
////	}
//	event->ignore();
//}




void WidgetPatchCord::focusInEvent(QFocusEvent* event)
{
//	else
	{
		redraw();
	}
}


void WidgetPatchCord::focusOutEvent(QFocusEvent* event)
{
	redraw();
}


//bool WidgetPatchCord::trySettingUnconnectedLet(WidgetLet* let)
//{
//	if (!isPartiallyConnected())
//	{
//		hm_error("trySettingsUnconnectedPoint called on WidgetPatchCord that is not partially connected,");
//		assert(isPartiallyConnected());
//		return false;
//	}
//	if (mInlet==nullptr)
//	{
//		WidgetInlet* inlet = dynamic_cast<WidgetInlet*>(let);
//		if (inlet && (inlet->inlet()->types() & mOutlet->outlet()->types()))
//		{
//			setInlet(inlet);
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//	else
//	{
//		assert(mOutlet==nullptr);
//		WidgetOutlet* outlet = dynamic_cast<WidgetOutlet*>(let);
//		if (outlet && (mInlet->inlet()->types() & outlet->outlet()->types()))
//		{
//			setOutlet(outlet);
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//}


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


//void WidgetPatchCord::setUnconnectedPointDrawPosition(QPoint position)
//{
////	qDebug() << "setUnconnected..";
//	assert((mOutlet==nullptr) != (mInlet==nullptr));
//	
//	if (mOutlet==nullptr)
//	{
//		mLine.setP1(position);
//	}
//	else
//	{
//		mLine.setP2(position);
//	}
//	redraw();
//}



void WidgetPatchCord::connectOutletSignals()
{
    assert(mOutlet);
    if (mOutlet)
    {
        bool success(true);
        
        success = connect(mOutlet, SIGNAL(positionChanged()), this, SLOT(redraw()));
        assert(success);
		success = connect(mOutlet->node(), SIGNAL(geometryChanged()), this, SLOT(redraw()));
		assert(success);
    }
}

void WidgetPatchCord::connectInletSignals()
{
    assert(mInlet);
    if (mInlet)
    {
        bool success(true);
        
        success = connect(mInlet, SIGNAL(positionChanged()), this, SLOT(redraw()));
        assert(success);
		success = connect(mInlet->node(), SIGNAL(geometryChanged()), this, SLOT(redraw()));
		assert(success);
    }
}

void WidgetPatchCord::disconnectOutletSignals()
{
    assert(mOutlet);
    
    if (mOutlet)
    {
        bool success(true);
        
        success = disconnect(mOutlet, SIGNAL(positionChanged()), this, SLOT(redraw()));
        assert(success);
		success = disconnect(mOutlet->node(), SIGNAL(geometryChanged()), this, SLOT(redraw()));
		assert(success);
		

    }
}

void WidgetPatchCord::disconnectInletSignals()
{
    assert(mInlet);
    
    if (mInlet)
    {
        bool success(true);
        
        success = disconnect(mInlet, SIGNAL(positionChanged()), this, SLOT(redraw()));
        assert(success);
 		success = disconnect(mInlet->node(), SIGNAL(geometryChanged()), this, SLOT(redraw()));
		assert(success);
    }
}

//void WidgetPatchCord::disconnectUnderlyingModel()
//{
//	assert(mInlet);
//    assert(mOutlet);
//	OutletPtr outlet = mOutlet->outlet();
//	InletPtr inlet = mInlet->inlet();
//	assert(mPatchArea->pipeline()->isConnected(outlet, inlet));
//	mPatchArea->pipeline()->disconnect(outlet, inlet);
//	hm_info(outlet->toString()+" disconnected via WidgetPatchCord from "+inlet->toString());
//}

//void WidgetPatchCord::connectUnderlyingModel()
//{
//	assert(mInlet);
//    assert(mOutlet);
//	OutletPtr outlet = mOutlet->outlet();
//	InletPtr inlet = mInlet->inlet();
//	assert(!mPatchArea->pipeline()->isConnected(outlet, inlet));
//    mPatchArea->pipeline()->connect(outlet, inlet);
//	hm_info(outlet->toString()+" connected via WidgetPatchCord to "+inlet->toString());
//}

//void WidgetPatchCord::setOutlet(WidgetOutlet* outlet)
//{
//    if (mOutlet)
//    {
//        disconnectOutletSignals();
//        if (mInlet)
//        {
//            disconnectUnderlyingModel();
//        }
//    }
//    mOutlet = outlet;
//	if (mOutlet)
//	{
//		hm_debug("WidgetPatchCord: outlet set to "+mOutlet->outlet()->toString());
//	}
//	else
//	{
//		hm_debug("WidgetPatchCord: outlet set to nullptr");
//	}
//    if (mOutlet)
//    {
//        connectOutletSignals();
//        // connect on the underlying model
//        if (mInlet)
//        {
//            connectUnderlyingModel();
//        }
//		show();
//    }
//    redraw();
//}

//void WidgetPatchCord::setInlet(WidgetInlet* inlet)
//{
//    if (mInlet)
//    {
//        disconnectInletSignals();
//        // if mOutlet is defined then this instance represents an existing
//        // connection
//        if (mOutlet)
//        {
//            disconnectUnderlyingModel();
//        }
//    }
//    mInlet = inlet;
//    if (mInlet)
//    {
//        connectInletSignals();
//        if (mOutlet)
//        {
//            connectUnderlyingModel();
//        }
//		show();
//    }
//    redraw();
//}


//bool WidgetPatchCord::isPartiallyConnected() const
//{
//    // mouse interaction is active iff we have only one of (inlet, outlet)
//    return (mInlet==nullptr) != (mOutlet==nullptr);
//}


//bool WidgetPatchCord::isFullyConnected() const
//{
//	return mInlet!=nullptr && mOutlet!=nullptr;
//}


