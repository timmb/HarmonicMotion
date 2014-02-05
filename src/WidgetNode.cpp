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
#include "WidgetParameter.h"
#include <QFile>
#include <QTimer>
#include "Common.h"
#include <QStyleOption>
#include <QPainter>

namespace hm
{
	
	WidgetNode::WidgetNode(NodePtr node, QWidget* parent)
	: QWidget(parent)
	, mNode(node)
	{
		loadStyleSheet();
		setObjectName("WidgetNode");
		QLabel* type = new QLabel(QString::fromUtf8(mNode->type().c_str()));
		type->setObjectName("LabelNodeType");
		QLineEdit* name = new QLineEdit(QString::fromUtf8(mNode->name().c_str()));
		// TODO: wait for Qt5
		//	bool success = connect(name, &QLineEdit::textChanged, [this](QString const& str)
		//			{
		//				mNode->setName(str->toUtf8());
		//			});
		//	assert(success);
		
		setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//		resize(500, 500);
		
		QGridLayout* layout = new QGridLayout;
		setLayout(layout);
		layout->addWidget(type, 0, 0);
		layout->addWidget(name, 0, 1);
		
		int row = 1;
		for (ParameterPtr p : node->parameters())
		{
			QWidget* widget = WidgetBaseParameter::create(p);
			layout->addWidget(new QLabel(QString::fromUtf8(p->name().c_str())), row, 0, Qt::AlignRight);
			layout->addWidget(widget, row, 1, Qt::AlignLeft);
			row++;
		}
		layout->setRowStretch(row, 1);
		
		QTimer* t = new QTimer(this);
		t->setInterval(500);
		connect(t, SIGNAL(timeout()), this, SLOT(loadStyleSheet()));
		t->start();
		
	}
	
	void WidgetNode::loadStyleSheet()
	{
		//	QFile file(":/qss/WidgetNode.qss");
		QFile file("/Users/tim/Documents/Programming/HarmonicMotion/HarmonicMotionGui/resources/qss/WidgetNode.qss");
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			setStyleSheet(QString::fromUtf8(file.readAll()));
		}
		else
		{
			hm_error("Failed to load stylesheet WidgetNode.qss");
		}
	}
	
	void WidgetNode::paintEvent(QPaintEvent *)
	{
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	}
	
	void WidgetNode::mousePressEvent(QMouseEvent* event)
	{
		event->accept(); // do not propagate
		if (isWindow())
			mDragOffset = event->globalPos() - pos();
		else
			mDragOffset = event->pos();
	}
	
	void WidgetNode::mouseMoveEvent(QMouseEvent* event)
	{
		event->accept(); // do not propagate
		if (isWindow())
			move(event->globalPos() - mDragOffset);
		else
			move(mapToParent(event->pos() - mDragOffset));
	}
	
	void WidgetNode::mouseReleaseEvent(QMouseEvent* event)
	{
		event->accept(); // do not propagate
		mDragOffset = QPoint();
	}
}
