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
    , mIsDragging(false)
	, mNode(node)
    , mMainArea(nullptr)
    , mPatchArea(patchArea)
	{
		loadStyleSheet();
		setObjectName("WidgetNode");
        
//        mInnerBox = new QWidget(this);
//        mInnerBox->setObjectName("InnerBox");
		QLabel* type = new QLabel(str(mNode->type()));
		type->setObjectName("LabelNodeType");
		QLineEdit* name = new QLineEdit(str(mNode->name()));
		// TODO: wait for Qt5
		//	bool success = connect(name, &QLineEdit::textChanged, [this](QString const& str)
		//			{
		//				mNode->setName(str->toUtf8());
		//			});
		//	assert(success);
		
//        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		
		QGridLayout* mainLayout = new QGridLayout;
        QVBoxLayout* inletsLayout = new QVBoxLayout;
        QVBoxLayout* outletsLayout = new QVBoxLayout;
        
        inletsLayout->setAlignment(Qt::AlignTop);
        outletsLayout->setAlignment(Qt::AlignTop);
        
//		mInnerBox->setLayout(layout);
		mainLayout->addWidget(type, 0, 0);
		mainLayout->addWidget(name, 0, 1);
		
		int row = 1;
		for (ParameterPtr p : node->parameters())
		{
			QWidget* widget = WidgetBaseParameter::create(p);
			mainLayout->addWidget(new QLabel(str(p->name())), row, 0, Qt::AlignRight);
			mainLayout->addWidget(widget, row, 1, Qt::AlignLeft);
			row++;
		}
		mainLayout->setRowStretch(row, 1);
		
		assert(parentWidget() != nullptr);
		for (InletPtr inlet: mNode->inlets())
		{
			WidgetInlet* w = new WidgetInlet(inlet, this);
			mWidgetInlets.push_back(w);
            inletsLayout->addWidget(w);
		}
		for (OutletPtr outlet: mNode->outlets())
		{
			WidgetOutlet* w = new WidgetOutlet(outlet, this);
			mWidgetOutlets.push_back(w);
            outletsLayout->addWidget(w);
		}
		
		connect(this, SIGNAL(geometryChanged()), this, SLOT(layout()));
		connect(this, SIGNAL(geometryChanged()), patchArea, SLOT(updateSize()));
		preventNegativePosition();
		Q_EMIT geometryChanged();
        
        mMainArea = new QWidget;
        mMainArea->setObjectName("mMainArea");
        mMainArea->setLayout(mainLayout);
		
        QHBoxLayout* layout = new QHBoxLayout;
        layout->addLayout(inletsLayout);
//        layout->addLayout(mainLayout);
        layout->addWidget(mMainArea);
        layout->addLayout(outletsLayout);
        layout->setSpacing(0);
        layout->setSizeConstraint(QLayout::SetFixedSize);
        setLayout(layout);
        
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
    
//    WidgetInlet* WidgetNode::widgetInlet(InletPtr inlet)
//    {
//        for (auto w: mWidgetInlets)
//        {
//            if (w->inlet() == inlet)
//            {
//                return w;
//            }
//        }
//        return nullptr;
//    }
//    
//    WidgetOutlet* WidgetNode::widgetOutlet(OutletPtr outlet)
//    {
//        for (auto w: mWidgetOutlets)
//        {
//            if (w->outlet() == outlet)
//            {
//                return w;
//            }
//        }
//        return nullptr;
//    }
//    
    QSize WidgetNode::sizeHint() const
    {
        QSize size = mMainArea->sizeHint();
        return QSize(size.width()
                     + (mWidgetOutlets.empty()? 0 : mWidgetOutlets[0]->width())
                     + (mWidgetInlets.empty()? 0 : mWidgetInlets[0]->width()),
                     size.height());
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
		mIsDragging = mMainArea->geometry().contains(event->pos());
        if (mIsDragging)
        {
            event->accept(); // do not propagate
            if (isWindow())
                mDragOffset = event->globalPos() - pos();
            else
                mDragOffset = event->pos();
        }
        else
        {
            event->ignore();
        }
	}
	
	void WidgetNode::mouseMoveEvent(QMouseEvent* event)
	{
        if (mIsDragging)
        {
            event->accept(); // do not propagate
            if (isWindow())
                move(event->globalPos() - mDragOffset);
            else
                move(mapToParent(event->pos() - mDragOffset));
        }
	}
	
	void WidgetNode::mouseReleaseEvent(QMouseEvent* event)
	{
        if (mIsDragging)
        {
            event->accept(); // do not propagate
            mDragOffset = QPoint();
        }
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
	
	void WidgetNode::layout()
	{
//        QPoint p(0,0);
//        for (auto w: mWidgetInlets)
//        {
//            w->move(p);
//            p += QPoint(0, w->height());
//        }
//        if (!mWidgetInlets.empty())
//        {
//            p = QPoint(0, mWidgetInlets[0]->width());
//        }
//        mInnerBox->move(p);
//        p += QPoint(0, mInnerBox->width());
//        for (auto w: mWidgetInlets)
//        {
//            w->move(p);
//            p += QPoint(0, w->height());
//        }
//        
//		QPoint p;
//		if (mWidgetInlets.size() > 0)
//		{
//			p = mapToParent(QPoint(- mWidgetInlets[0]->width(), 0));
//			for (auto w: mWidgetInlets)
//			{
//				w->move(p);
//				p += QPoint(0, w->height());
//			}
//		}
//		
//		p = mapToParent(QPoint(width(), 0));
//		for (auto w: mWidgetOutlets)
//		{
//			w->move(p);
//			p += QPoint(0, w->height());
//		}
	}
}
