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
		/// WidgetNode should only be destroyed by WidgetPatchArea.
		virtual ~WidgetNode();
		
		/// Request that this node is deleted (from both view and model).
		/// This instance will be deleted on the next cycle of the event
		/// loop.
		void erase();
		/// Request that this node is deleted without making the
		/// corresponding change to the underlying model. Used if the
		/// model has been updated externally.
		void eraseWithoutUpdatingModel();
        
        /// The widgets for the inlets or outlets to this class. These are
        /// guaranteed not to change over the course of the widget's life.
        QVector<WidgetInlet*> inlets() const { return mWidgetInlets; }
        QVector<WidgetOutlet*> outlets() const { return mWidgetOutlets; }
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
		virtual void focusInEvent(QFocusEvent* event) override;
		virtual void focusOutEvent(QFocusEvent* event) override;
        virtual QSize sizeHint() const override;
		
	Q_SIGNALS:
		void geometryChanged();
		/// Emitted when this node has gained focus and so the info panel
		/// needs updating
		void newInfoPanelText(QString);
		
	protected Q_SLOTS:
		void loadStyleSheet();
		void layout();
		/// Removes this widgetnode from the patcharea, removes the corresponding
		/// node from the model and deletes this widgetnode.
		void eraseAndDelete();

		
	private:
		void preventNegativePosition();
		
		// Owning reference
		QVector<class WidgetOutlet*> mWidgetOutlets;
		// Owning reference
		QVector<class WidgetInlet*> mWidgetInlets;
		QPoint mDragOffset;
        bool mIsDragging;
		NodePtr mNode;
        QWidget* mMainArea;
        WidgetPatchArea* mPatchArea;
		bool mHasBeenErased;
	};
}