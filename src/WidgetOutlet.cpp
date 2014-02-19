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

using namespace hm;

WidgetOutlet::WidgetOutlet(OutletPtr outlet, WidgetNode* parent)
: WidgetLet(parent)
, mOutlet(outlet)
{
    // for CSS
	setObjectName("WidgetOutlet");
    // for WidgetPatchArea to find outlets
    setProperty("isOutlet", true);
	QIcon icon(":/icons/outlet.svg");
	setPixmap(icon.pixmap(20, 20));
	
	loadStyleSheet();
	
	// TODO: temp
	QTimer* t = new QTimer;
	connect(t, SIGNAL(timeout()), this, SLOT(loadStyleSheet()));
	t->setInterval(500);
	t->start();
}


void WidgetOutlet::loadStyleSheet()
{
	//	QFile file(":/qss/WidgetNode.qss");
	QFile file("/Users/timmb/Documents/Programming/HarmonicMotion/HarmonicMotionGui/resources/qss/WidgetLet.qss");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		setStyleSheet(QString::fromUtf8(file.readAll()));
	}
	else
	{
		hm_error("Failed to load stylesheet WidgetLet.qss");
	}
}


void WidgetOutlet::mousePressEvent(QMouseEvent* event)
{
    if (event->button()==Qt::LeftButton)
    {
        event->accept();
    }
}