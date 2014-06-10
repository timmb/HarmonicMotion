//
//  WidgetNewPatchCord.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/05/2014.
//
//

#include "WidgetNewPatchCord.h"
#include "WidgetPatchArea.h"
#include "WidgetInlet.h"
#include "WidgetOutlet.h"
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QCursor>

using namespace hm;

WidgetNewPatchCord::WidgetNewPatchCord(WidgetInlet* inlet, WidgetPatchArea* patchArea)
: QWidget(patchArea)
, mPatchArea(patchArea)
, mInlet(inlet)
, mOutlet(nullptr)
{
	assert(mInlet != nullptr);
	assert(mPatchArea != nullptr);
	init();
}

WidgetNewPatchCord::WidgetNewPatchCord(WidgetOutlet* outlet, WidgetPatchArea* patchArea)
: QWidget(patchArea)
, mPatchArea(patchArea)
, mInlet(nullptr)
, mOutlet(outlet)
{
	assert(mOutlet != nullptr);
	assert(mPatchArea != nullptr);
	init();
}

WidgetNewPatchCord::~WidgetNewPatchCord()
{
	setMouseTracking(false);
}

void WidgetNewPatchCord::init()
{
	setFocusPolicy(Qt::StrongFocus);
	setGeometry(QRect(0, 0, mPatchArea->width(), mPatchArea->height()));
	if (mInlet != nullptr)
	{
		mLine.setP1(mInlet->connectionPoint());
	}
	else
	{
		assert(mOutlet != nullptr);
		mLine.setP1(mOutlet->connectionPoint());
	}
	QPoint mousePos = mPatchArea->mapFromGlobal(QCursor::pos());
	mLine.setP2(mousePos);
	setMouseTracking(true);
	show();
	setFocus(Qt::MouseFocusReason);
}

void WidgetNewPatchCord::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(QPen(QColor(50,50,50)));
	QLine line(mLine.p1(), mLine.p2());
	painter.drawLine(line);
}

void WidgetNewPatchCord::mouseMoveEvent(QMouseEvent* event)
{
	mLine.setP2(event->pos());
	update();
}

void WidgetNewPatchCord::mousePressEvent(QMouseEvent* event)
{
	if (mInlet == nullptr)
	{
		mInlet = mPatchArea->findInlet(event->pos());
	}
	else if (mOutlet == nullptr)
	{
		qDebug() << "findoutlet"<<event->pos()<<"my geometry"<<geometry()
		<<"patch area"<<mPatchArea->geometry();

		mOutlet = mPatchArea->findOutlet(event->pos());
	}
	if (mInlet != nullptr && mOutlet != nullptr)
	{
		mPatchArea->pipeline()->connect(mOutlet->outlet(), mInlet->inlet());
		mPatchArea->clearNewPatchCord(this);
	}
}


void WidgetNewPatchCord::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape)
	{
		mPatchArea->clearNewPatchCord(this);
	}
}
