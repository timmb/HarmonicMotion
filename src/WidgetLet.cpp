//
//  WidgetLet.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 10/02/2014.
//
//

#include "WidgetLet.h"
#include "WidgetNode.h"
#include <cassert>
#include "WidgetPatchArea.h"
#include <QCoreApplication>

using namespace hm;

WidgetLet::WidgetLet(WidgetNode* parent)
: QLabel(parent)
, mParent(parent)
{
    assert(mParent != nullptr);
}

WidgetLet::~WidgetLet()
{
	hm_debug("WidgetLet destructor");
}

QPoint WidgetLet::connectionPoint() const
{
    return mapTo(mParent->patchArea(), QPoint(width()/2, height()/2));
}

void WidgetLet::moveEvent(QMoveEvent*)
{
    Q_EMIT positionChanged();
}

void WidgetLet::resizeEvent(QResizeEvent*)
{
    Q_EMIT positionChanged();
}


void WidgetLet::mousePressEvent(QMouseEvent* event)
{
    if (event->button()==Qt::LeftButton)
    {
        // forward to the patch area to potentially start new patchcord
		QPoint posRelativeToPatchArea = mapTo(node()->patchArea(), event->pos());
		node()->patchArea()->mousePressEventFromWidgetLet(this, posRelativeToPatchArea);
		event->accept();
    }
}

