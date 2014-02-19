//
//  WidgetNode.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 03/02/2014.
//
//

#pragma once
#include <QWidget>
#include "Common.h"
#include <QPaintEvent>

namespace hm
{
	class WidgetPatchArea;
	class WidgetInlet;
    class WidgetOutlet;
    
	class WidgetNode : public QWidget
	{
		Q_OBJECT;
	public:
		WidgetNode(NodePtr node, WidgetPatchArea* patchArea=nullptr);
		virtual ~WidgetNode();
        
//        /// \return the WidgetInlet for the specified inlet if it belongs
//        /// to this WidgetNode, or nullptr if not.
//        WidgetInlet* widgetInlet(InletPtr inlet);
//        /// \return the WidgetOutlet for the specified outlet if it belongs
//        /// to this WidgetNode, or nullptr if not.
//        WidgetOutlet* widgetOutlet(OutletPtr outlet);
        
        /// The widgets for the inlets or outlets to this class. These are
        /// guaranteed not to change over the course of the widget's life.
        QVector<WidgetInlet*> widgetInlets() const { return mWidgetInlets; }
        QVector<WidgetOutlet*> widgetOutlets() const { return mWidgetOutlets; }
        NodePtr node() const { return mNode; }
        /// \return A pointer to the WidgetPatchArea that contains this
        /// WidgetNode
        WidgetPatchArea* patchArea() const { return mPatchArea; }
		
	protected:
		/// Necessary to reimplement to make stylesheet work
		/// see http://stackoverflow.com/questions/7276330/qt-stylesheet-for-custom-widget
		virtual void paintEvent(QPaintEvent*) override;
		// For drag and drop
		virtual void mousePressEvent(QMouseEvent* event) override;
		virtual void mouseMoveEvent(QMouseEvent* event) override;
		virtual void mouseReleaseEvent(QMouseEvent* event) override;
		virtual void resizeEvent(QResizeEvent* event) override;
		virtual void moveEvent(QMoveEvent* event) override;
        virtual QSize sizeHint() const override;
		
	Q_SIGNALS:
		void geometryChanged();
		
	protected Q_SLOTS:
		void loadStyleSheet();
		void layout();
		

		
	private:
		void preventNegativePosition();
		
		QVector<class WidgetOutlet*> mWidgetOutlets;
		QVector<class WidgetInlet*> mWidgetInlets;
		QPoint mDragOffset;
        bool mIsDragging;
		NodePtr mNode;
        QWidget* mMainArea;
        WidgetPatchArea* mPatchArea;
	};
}