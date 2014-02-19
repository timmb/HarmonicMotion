//
//  WidgetInlet.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 10/02/2014.
//
//

#include "WidgetInlet.h"
#include <QFile>
#include <QIcon>
#include <QTimer>
#include <QDebug>

using namespace hm;

WidgetInlet::WidgetInlet(InletPtr inlet, QWidget* parent)
: WidgetLet(parent)
, mInlet(inlet)
{
	setObjectName("WidgetOutlet");
	QIcon icon(":/icons/inlet.svg");
	setPixmap(icon.pixmap(20, 20));
	
	loadStyleSheet();
	
	// TODO: temp
	QTimer* t = new QTimer;
	connect(t, SIGNAL(timeout()), this, SLOT(loadStyleSheet()));
	t->setInterval(500);
	t->start();
}


void WidgetInlet::mousePressEvent(QMouseEvent * e)
{
    qDebug() << "Mouse pressed on inlet.";
}


void WidgetInlet::loadStyleSheet()
{
	//TODO: make resource
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

