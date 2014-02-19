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
#include "Utilities.h"
#include "WidgetOutlet.h"
#include <algorithm>
#include "WidgetPatchArea.h"
#include "WidgetInlet.h"

namespace hm
{
	
	WidgetNode::WidgetNode(NodePtr node, WidgetPatchArea* patchArea)
	: QWidget(patchArea)
	, mNode(node)
	{
		loadStyleSheet();
		setObjectName("WidgetNode");
		QLabel* type = new QLabel(str(mNode->type()));
		type->setObjectName("LabelNodeType");
		QLineEdit* name = new QLineEdit(str(mNode->name()));
		// TODO: wait for Qt5
		//	bool success = connect(name, &QLineEdit::textChanged, [this](QString const& str)
		//			{
		//				mNode->setName(str->toUtf8());
		//			});
		//	assert(success);
		
		setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		
		QGridLayout* layout = new QGridLayout;
		setLayout(layout);
		layout->addWidget(type, 0, 0);
		layout->addWidget(name, 0, 1);
		
		int row = 1;
		for (ParameterPtr p : node->parameters())
		{
			QWidget* widget = WidgetBaseParameter::create(p);
			layout->addWidget(new QLabel(str(p->name())), row, 0, Qt::AlignRight);
			layout->addWidget(widget, row, 1, Qt::AlignLeft);
			row++;
		}
		layout->setRowStretch(row, 1);
		
		assert(parentWidget() != nullptr);
		for (OutletPtr outlet: mNode->outlets())
		{
			WidgetOutlet* w = new WidgetOutlet(outlet, parentWidget());
			mWidgetOutlets.push_back(w);
		}
		for (InletPtr inlet: mNode->inlets())
		{
			WidgetInlet* w = new WidgetInlet(inlet, parentWidget());
			mWidgetInlets.push_back(w);
		}
		
		connect(this, SIGNAL(geometryChanged()), this, SLOT(arrangeLets()));
		connect(this, SIGNAL(geometryChanged()), patchArea, SLOT(updateSize()));
		preventNegativePosition();
		Q_EMIT geometryChanged();
		
		// TODO: temp
		QTimer* t = new QTimer(this);
		t->setInterval(500);
		connect(t, SIGNAL(timeout()), this, SLOT(loadStyleSheet()));
		t->start();
	}
	
	WidgetNode::~WidgetNode()
	{
//		for (auto w: mWidgetOutlets)
//		{
//			assert(w->parent() == nullptr);
//			delete w;
//		}
//		mWidgetOutlets.clear();
	}
	
	void WidgetNode::loadStyleSheet()
	{
		//	QFile file(":/qss/WidgetNode.qss");
		QFile file("/Users/timmb/Documents/Programming/HarmonicMotion/HarmonicMotionGui/resources/qss/WidgetNode.qss");
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
	
	void WidgetNode::resizeEvent(QResizeEvent* event)
	{
		Q_EMIT geometryChanged();
	}
	
	void WidgetNode::moveEvent(QMoveEvent* event)
	{
		preventNegativePosition();
		Q_EMIT geometryChanged();
	}
	
	void WidgetNode::preventNegativePosition()
	{
		// prevent widgets from having negative positions (relative to the
		// patch area
		if (pos().x() < 0 || pos().y() < 0)
		{
			move(QPoint(std::max(0, pos().x()),
						std::max(0, pos().y())));
			return;
		}
	}
	
	void WidgetNode::arrangeLets()
	{
		QPoint p;
		if (mWidgetInlets.size() > 0)
		{
			p = mapToParent(QPoint(- mWidgetInlets[0]->width(), 0));
			for (auto w: mWidgetInlets)
			{
				w->move(p);
				p += QPoint(0, w->height());
			}
		}
		
		p = mapToParent(QPoint(width(), 0));
		for (auto w: mWidgetOutlets)
		{
			w->move(p);
			p += QPoint(0, w->height());
		}
	}
}
