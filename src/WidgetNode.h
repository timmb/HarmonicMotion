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
	
	class WidgetNode : public QWidget
	{
		Q_OBJECT;
	public:
		WidgetNode(NodePtr node, WidgetPatchArea* patchArea=nullptr);
		virtual ~WidgetNode();
		
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
	};
}