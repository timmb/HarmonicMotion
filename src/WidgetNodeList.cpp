//
//  WidgetNodeList.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 06/02/2014.
//
//

#include "Common.h"
#include "WidgetNodeList.h"
#include <QStyleOption>
#include <QPainter>
#include <QFile>
#include "FactoryNode.h"
#include "WidgetNodePrototype.h"
#include <QBoxLayout>
#include <QScrollArea>
#include <QLabel>

using namespace hm;

WidgetNodeList::WidgetNodeList(MainWindow* mainWindow, QWidget* parent)
: QDockWidget("Nodes", parent)
{
	loadStyleSheet();
	QBoxLayout* layout = new QVBoxLayout;
	
	// Iterate through nodes registered with factory and create prototype
	// widgets for them
	std::vector<NodeInfo> nodes = FactoryNode::instance()->nodeTypes();
	for (NodeInfo const& info: nodes)
	{
		WidgetNodePrototype* p = new WidgetNodePrototype(info, mainWindow);
		layout->addWidget(p);
	}
	
	QWidget* centralWidget = new QWidget;
	centralWidget->setLayout(layout);

	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidget(centralWidget);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
	setWidget(scrollArea);
//	setWidget(centralWidget);
//	show();
}

void WidgetNodeList::loadStyleSheet()
{
	//	QFile file(":/qss/WidgetNode.qss");
	QFile file("/Users/tim/Documents/Programming/HarmonicMotion/HarmonicMotionGui/resources/qss/WidgetNodeList.qss");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		setStyleSheet(QString::fromUtf8(file.readAll()));
	}
	else
	{
		hm_error("Failed to load stylesheet WidgetNodeList.qss");
	}
}

void WidgetNodeList::paintEvent(QPaintEvent *)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
