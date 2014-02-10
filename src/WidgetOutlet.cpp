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

using namespace hm;

WidgetOutlet::WidgetOutlet(OutletPtr outlet, QWidget* parent)
: WidgetLet(parent)
, mOutlet(outlet)
{
	setObjectName("WidgetOutlet");
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
	QFile file("/Users/tim/Documents/Programming/HarmonicMotion/HarmonicMotionGui/resources/qss/WidgetOutlet.qss");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		setStyleSheet(QString::fromUtf8(file.readAll()));
	}
	else
	{
		hm_error("Failed to load stylesheet WidgetOutlet.qss");
	}
}
