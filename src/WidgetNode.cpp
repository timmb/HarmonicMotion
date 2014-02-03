//
//  WidgetNode.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 03/02/2014.
//
//

#include "WidgetNode.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include "Node.h"

namespace hm
{

WidgetNode::WidgetNode(NodePtr node, QWidget* parent)
: QWidget(parent)
, mNode(node)
{
	QLabel* type = new QLabel(QString::fromUtf8(mNode->type().c_str()));
	QLineEdit* name = new QLineEdit(QString::fromUtf8(mNode->name().c_str()));
	// wait for Qt5
//	bool success = connect(name, &QLineEdit::textChanged, [this](QString const& str)
//			{
//				mNode->setName(str->toUtf8());
//			});
//	assert(success);
	
	QGridLayout* layout = new QGridLayout;
	setLayout(layout);
	layout->addWidget(type, 0, 0);
	layout->addWidget(name, 0, 1);
	
	
}

}