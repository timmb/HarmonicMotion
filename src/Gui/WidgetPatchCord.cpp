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

WidgetPatchCord::WidgetPatchCord(WidgetPatchArea* patchArea, WidgetOutlet* outlet, WidgetInlet* inlet)
: QWidget(patchArea)
, mOutlet(outlet)
, mInlet(inlet)
, mPatchArea(patchArea)
//, mHasBeenErased(false)
{
    assert(mPatchArea != nullptr);
    assert(mOutlet != nullptr);
//    assert(mOutlet->outlet()->isConnectedTo(mInlet->inlet()));
    assert(mInlet != nullptr);
    connectInletSignals();
    connectOutletSignals();
    redraw();
	mDescription = "WidgetPatchCord "+mOutlet->outlet()->toString()+" -> inlet "+mInlet->inlet()->toString();
	hm_debug("Created "+mDescription);
	
	setAttribute(Qt::WA_TransparentForMouseEvents);
	
	// ACTIONS
	QAction* eraseAction = new QAction("Delete", this);
	eraseAction->setShortcuts(QList<QKeySequence>() << QKeySequence::Delete << Qt::Key_Backspace);
	eraseAction->setShortcutContext(Qt::WidgetShortcut);
	addAction(eraseAction);
	bool success = connect(eraseAction, SIGNAL(triggered()), this, SLOT(deleteFromModel()));
	assert(success);
	
	show();
}

WidgetPatchCord::~WidgetPatchCord()
{
//	if (!mHasBeenErased)
//	{
//		hm_error("WidgetPatchCord destroyed before being erased.");
//		assert(mHasBeenErased);
//	}
	hm_debug("Destroyed "+mDescription);
}

//
//void WidgetPatchCord::loadStyleSheet()
//{
//	QFile file("/Users/timmb/Documents/Programming/HarmonicMotion/Gui/resources/qss/WidgetPatchCord.qss");
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


void WidgetPatchCord::deleteFromModel()
{
	bool success = mPatchArea->pipeline()->disconnect(mOutlet->outlet(), mInlet->inlet());
	assert(success && "Patchcord is removed from underlying model here.");
//	mHasBeenErased = true;
}


//void WidgetPatchCord::eraseWithoutUpdatingModel()
//{
//	mPatchArea->erasePatchCord(this);
//	mHasBeenErased = true;
//}


void WidgetPatchCord::redraw()
{
	assert (mInlet != nullptr && mOutlet != nullptr);
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
    update();
}



void WidgetPatchCord::paintEvent(QPaintEvent* event)
{
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
}


void WidgetPatchCord::myMousePressEvent(QMouseEvent const& event)
{
	setFocus(Qt::MouseFocusReason);
}


bool WidgetPatchCord::isLineNear(QPoint const& point) const
{
	QLine relLine(mLine.p1() - pos(), mLine.p2() - pos());
	double d = distanceFromPointToLine(QVector2D(relLine.p1()), QVector2D(relLine.p2()), QVector2D(point));
	return d < 5.f;
}



void WidgetPatchCord::focusInEvent(QFocusEvent* event)
{
		redraw();
}


void WidgetPatchCord::focusOutEvent(QFocusEvent* event)
{
	redraw();
}


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
		success = connect(mOutlet->node(), SIGNAL(beingDragged(WidgetNode*)), this, SLOT(redraw()));
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
		success = connect(mInlet->node(), SIGNAL(beingDragged(WidgetNode*)), this, SLOT(redraw()));
		assert(success);
    }
}

//void WidgetPatchCord::disconnectOutletSignals()
//{
//    assert(mOutlet);
//    
//    if (mOutlet)
//    {
//        bool success(true);
//        
//        success = disconnect(mOutlet, SIGNAL(positionChanged()), this, SLOT(redraw()));
//        assert(success);
//		success = disconnect(mOutlet->node(), SIGNAL(geometryChanged()), this, SLOT(redraw()));
//		assert(success);
//		
//
//    }
//}
//
//void WidgetPatchCord::disconnectInletSignals()
//{
//    assert(mInlet);
//    
//    if (mInlet)
//    {
//        bool success(true);
//        
//        success = disconnect(mInlet, SIGNAL(positionChanged()), this, SLOT(redraw()));
//        assert(success);
// 		success = disconnect(mInlet->node(), SIGNAL(geometryChanged()), this, SLOT(redraw()));
//		assert(success);
//    }
//}
