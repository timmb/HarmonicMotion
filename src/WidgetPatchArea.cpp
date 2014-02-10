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

void WidgetPatchArea::updateSize()
{
	QRect content = childrenRect();
	content.setLeft(0);
	content.setTop(0);
	content.setWidth(qMax(sizeHint().width(), content.width()));
	content.setHeight(qMax(sizeHint().height(), content.height()));
	qDebug() << "settings patcharea geometry to"<<content;
	setGeometry(content);
}