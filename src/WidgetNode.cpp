//
//  WidgetNode.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 03/02/2014.
//
//

#include "WidgetNode.h"
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
#include "Pipeline.h"
#include <QAction>

namespace hm
{
	
	WidgetNode::WidgetNode(NodePtr node, WidgetPatchArea* patchArea)
	: QWidget(patchArea)
    , mIsDragging(false)
	, mNode(node)
    , mMainArea(nullptr)
    , mPatchArea(patchArea)
//	, mHasBeenErased(false)
	, mNameWidget(nullptr)
	{
		assert(node != nullptr);
		
		loadStyleSheet();
		setObjectName("WidgetNode");
		setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        
		QLabel* typeLabel = new QLabel(str(mNode->type()));
		typeLabel->setObjectName("LabelNodeType");
		
		mNameWidget = new QLineEdit(str(mNode->name()));

		
        QVBoxLayout* inletsLayout = new QVBoxLayout;
        QVBoxLayout* outletsLayout = new QVBoxLayout;
        
        inletsLayout->setAlignment(Qt::AlignTop);
        outletsLayout->setAlignment(Qt::AlignTop);
        
		for (ParameterPtr p : node->parameters())
		{
			WidgetBaseParameter* widget = WidgetBaseParameter::create(p);
			QLabel* label = new QLabel(str(p->name()));
			mWidgetParameters.push_back(QPair<QLabel*, WidgetBaseParameter*>(label, widget));
			BOOST_VERIFY(connect(widget, SIGNAL(parameterVisibilityChanged(WidgetBaseParameter*, bool)), this, SLOT(parameterVisibilityChanged(WidgetBaseParameter*, bool))));
		}
		
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
		
		preventNegativePosition();
        
		// Setup main area
        mMainArea = new QWidget;
		mMainLayout = new QGridLayout;
		mMainArea->setLayout(mMainLayout);
		repaint();
		
		mMainLayout->addWidget(typeLabel, 0, 0);
		mMainLayout->addWidget(mNameWidget, 0, 1);
		for (QPair<QLabel*, WidgetBaseParameter*> p: mWidgetParameters)
		{
			int row = mMainLayout->rowCount();
			mMainLayout->addWidget(p.first, row, 0, Qt::AlignRight);
			mMainLayout->addWidget(p.second, row, 1, Qt::AlignLeft);
			if (p.second->isParameterVisible())
			{
				p.first->show();
				p.second->show();
			}
			else
			{
				p.first->hide();
				p.second->hide();
			}
		}
		mMainLayout->setRowStretch(mMainLayout->rowCount(), 1);
		update();
        mMainArea->setObjectName("mMainArea");
		setFocusPolicy(Qt::ClickFocus);
		setContextMenuPolicy(Qt::ActionsContextMenu);
		
		QHBoxLayout* layout = new QHBoxLayout(this);
        layout->addLayout(inletsLayout);
        layout->addWidget(mMainArea);
        layout->addLayout(outletsLayout);
        layout->setSpacing(0);
        layout->setSizeConstraint(QLayout::SetFixedSize);
//        setLayout(mLayout);
        
		// CONNECTIONS
		
		bool success(true);
		
		success = connect(mNameWidget, SIGNAL(editingFinished()), this, SLOT(nameChangedInGui()));
		assert(success);
		success = connect(this, SIGNAL(geometryChanged()), patchArea, SLOT(updateSize()));
		assert(success);
		success = connect(this, SIGNAL(newInfoPanelText(QString)), patchArea, SLOT(provideInfoPanelText(QString)));
		assert(success);
		
		// ACTIONS
		QAction* eraseAction = new QAction("Delete", this);
		eraseAction->setShortcuts(QList<QKeySequence>() << QKeySequence::Delete << Qt::Key_Backspace);
		eraseAction->setShortcutContext(Qt::WidgetShortcut);
		addAction(eraseAction);
		success = connect(eraseAction, SIGNAL(triggered()), this, SLOT(deleteFromModel()));
		assert(success);
		
		updateFromNodeParams();
		
		
		Q_EMIT geometryChanged();

//		// TODO: temp
//		QTimer* t = new QTimer(this);
//		t->setInterval(500);
//		connect(t, SIGNAL(timeout()), this, SLOT(loadStyleSheet()));
//		t->start();
	}

	
	void WidgetNode::parameterVisibilityChanged(WidgetBaseParameter* widget, bool isVisible)
	{
		for (QPair<QLabel*, WidgetBaseParameter*> p: mWidgetParameters)
		{
			if (p.second->isParameterVisible())
			{
				p.first->show();
				p.second->show();
			}
			else
			{
				p.first->hide();
				p.second->hide();
			}
		}
	}
	
	WidgetNode::~WidgetNode()
	{
		hm_debug("WidgetNode destructor for node "+mNode->name());
	}
	
	void WidgetNode::nameChangedInGui()
	{
		mNode->setName(mNameWidget->text().toStdString());
	}
	
	void WidgetNode::updateFromNodeParams()
	{
		Node::Params params = mNode->exportParams();
		move(params.guiLocationX, params.guiLocationY);
		mNameWidget->blockSignals(true);
		mNameWidget->setText(str(params.name));
		mNameWidget->blockSignals(false);
	}
	
	void WidgetNode::deleteFromModel()
	{
		mPatchArea->pipeline()->removeNode(mNode);
	}
    
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
		QFile file("/Users/timmb/Documents/Programming/HarmonicMotion/Gui/resources/qss/WidgetNode.qss");
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
		else
		{
			event->ignore();
		}
	}
	
	void WidgetNode::mouseReleaseEvent(QMouseEvent* event)
	{
        if (mIsDragging)
        {
            event->accept(); // do not propagate
            mDragOffset = QPoint();
			mIsDragging = false;
        }
	}
	
	void WidgetNode::resizeEvent(QResizeEvent* event)
	{
		Q_EMIT geometryChanged();
	}
	
	void WidgetNode::moveEvent(QMoveEvent* event)
	{
		preventNegativePosition();
		// update values in model and allow it to change those values.
		QPoint pos = event->pos();
		Node::Params params = node()->exportParams();
		params.guiLocationX = pos.x();
		params.guiLocationY = pos.y();
		// this function may modify `params`
		node()->setNodeParams(params);
		if (params.guiLocationX != pos.x() || params.guiLocationY != pos.y())
		{
			assert(params.guiLocationX >= 0 && params.guiLocationY >= 0);
			// there's no reason why node will have set x or y to be negative
			// but it's essential they're non-negative as otherwise we
			// may enter an infinite recursion where this class and Node
			// continually adjust each other's position.
			pos.setX(qMax(0, pos.x()));
			pos.setY(qMax(0, pos.y()));
			move(pos);
			return;
		}
		Q_EMIT geometryChanged();
	}
	
	void WidgetNode::focusInEvent(QFocusEvent* event)
	{
		mPatchArea->raise(this);
		/// this property adds the black border around the widget
		mMainArea->setProperty("hasFocus", true);
		// force update of appearance
		repolish(mMainArea);
		Q_EMIT newInfoPanelText(str(mNode->toString()));
	}
	
	void WidgetNode::focusOutEvent(QFocusEvent* event)
	{
		mMainArea->setProperty("hasFocus", false);
		// force update of appearance
		repolish(mMainArea);
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
	
	void WidgetNode::addWidget(QWidget* widget)
	{
		mMainLayout->addWidget(widget, mMainLayout->rowCount(), 0, 1, mMainLayout->columnCount());
	}
	
//	void WidgetNode::layout()
//	{
////        QPoint p(0,0);
////        for (auto w: mWidgetInlets)
////        {
////            w->move(p);
////            p += QPoint(0, w->height());
////        }
////        if (!mWidgetInlets.empty())
////        {
////            p = QPoint(0, mWidgetInlets[0]->width());
////        }
////        mInnerBox->move(p);
////        p += QPoint(0, mInnerBox->width());
////        for (auto w: mWidgetInlets)
////        {
////            w->move(p);
////            p += QPoint(0, w->height());
////        }
////        
////		QPoint p;
////		if (mWidgetInlets.size() > 0)
////		{
////			p = mapToParent(QPoint(- mWidgetInlets[0]->width(), 0));
////			for (auto w: mWidgetInlets)
////			{
////				w->move(p);
////				p += QPoint(0, w->height());
////			}
////		}
////		
////		p = mapToParent(QPoint(width(), 0));
////		for (auto w: mWidgetOutlets)
////		{
////			w->move(p);
////			p += QPoint(0, w->height());
////		}
//	}
}


