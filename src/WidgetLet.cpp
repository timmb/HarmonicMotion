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

using namespace hm;

WidgetLet::WidgetLet(WidgetNode* parent)
: QLabel(parent)
, mParent(parent)
{
    assert(mParent != nullptr);
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