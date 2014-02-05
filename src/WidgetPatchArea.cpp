//
//  WidgetPatchArea.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 05/02/2014.
//
//

#include "WidgetPatchArea.h"

using namespace hm;

WidgetPatchArea::WidgetPatchArea(QWidget* parent)
: QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setAutoFillBackground(true);
//	mPatchArea->setColor(QPalette::Window, QColor("gray"));
}

QSize WidgetPatchArea::sizeHint() const
{
	return QSize(500, 300);
}