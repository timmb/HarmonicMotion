//
//  NodeOscOutWidget.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 24/10/2013.
//
//

#include "NodeOscOutWidget.h"
#include "NodeOscOut.h"
#include <QtGui>

using namespace hm;


NodeOscOutWidget::NodeOscOutWidget(NodeOscOutPtr nodeOscOut, QWidget* parent)
: QWidget(parent)
, mNode(nodeOscOut)
, mHostWidget(nullptr)
{
	auto params = mNode->params();
	
	auto layout = new QFormLayout;
	setLayout(layout);
	
	mHostWidget = new QLineEdit(this);
	mHostWidget->setText(QString::fromStdString(params.destinationHost));
	layout->addRow("Destination hostname", mHostWidget);
	bool success = connect(this, SIGNAL(newDestinationHost(QString)), mHostWidget, SLOT(setText(QString)));
	assert(success);
	success = connect(mHostWidget, SIGNAL(editingFinished()), this, SLOT(destinationHostWidgetChanged()));
	assert(success);
	
	auto port = new QSpinBox(this);
	port->setRange(1, 65535);
	port->setValue(params.destinationPort);
	layout->addRow("Port", port);
	success = connect(this, SIGNAL(newDestinationPort(int)), port, SLOT(setValue(int)));
	assert(success);
	success = connect(port, SIGNAL(valueChanged(int)), this, SLOT(setDestinationPort(int)));
	assert(success);
	
	auto prefix = new QLineEdit(this);
	prefix->setText(QString::fromStdString(params.prefix));
	layout->addRow("OSC address prefix", prefix);
	success = connect(this, SIGNAL(newPrefix(QString)), prefix, SLOT(setText(QString)));
	assert(success);
	success = connect(prefix, SIGNAL(textChanged(QString)), this, SLOT(setPrefix(QString)));
	assert(success);
	
}


void NodeOscOutWidget::destinationHostWidgetChanged()
{
	auto params = mNode->params();
	params.destinationHost = mHostWidget->text().toStdString();
	mNode->setParams(params);
}

void NodeOscOutWidget::setDestinationPort(int port)
{
	auto params = mNode->params();
	params.destinationPort = port;
	mNode->setParams(params);
}

void NodeOscOutWidget::setPrefix(QString prefix)
{
	auto params = mNode->params();
	params.prefix = prefix.toStdString();
	mNode->setParams(params);
}