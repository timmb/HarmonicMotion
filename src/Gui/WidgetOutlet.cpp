//
//  WidgetOutlet.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 10/02/2014.
//
//

#include "WidgetOutlet.h"
#include "Outlet.h"
#include <QIcon>
#include <QTimer>
#include <QFile>
#include <QMouseEvent>
#include "WidgetNode.h"
#include "WidgetPatchArea.h"

using namespace hm;

WidgetOutlet::WidgetOutlet(OutletPtr outlet, WidgetNode* parent)
: WidgetLet(outlet, parent)
, mOutlet(outlet)
{
	assert(mOutlet != nullptr);
    // for CSS and WidgetPatchArea
	setObjectName("WidgetOutlet");
	QIcon icon(":/icons/outlet.png");
	setPixmap(icon.pixmap(20, 20));
	
	loadStyleSheet();
	
//	// TODO: temp
//	QTimer* t = new QTimer;
//	connect(t, SIGNAL(timeout()), this, SLOT(loadStyleSheet()));
//	t->setInterval(500);
//	t->start();
}


void WidgetOutlet::loadStyleSheet()
{
	QFile file(":/qss/WidgetLet.qss");
//	QFile file("/Users/timmb/Documents/Programming/HarmonicMotion/Gui/resources/qss/WidgetLet.qss");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		setStyleSheet(QString::fromUtf8(file.readAll()));
	}
	else
	{
		hm_error("Failed to load stylesheet WidgetLet.qss");
	}
}

