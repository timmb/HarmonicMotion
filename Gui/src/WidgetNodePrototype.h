//
//  WidgetNodePrototype.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 06/02/2014.
//
//

#pragma once
#include <QWidget>
#include "FactoryNode.h"

namespace hm
{
	class MainWindow;
	
	/// A prototype widget listened in the NodeList for the user to select
	/// and create an instance of that node type
	class WidgetNodePrototype : public QWidget
	{
		Q_OBJECT
	public:
		/// \param mainWindow The main window into which newly created nodes will
		/// be created.
		WidgetNodePrototype(NodeInfo const& info, MainWindow* mainWindow, QWidget* parent=nullptr);
		
		NodeInfo nodeInfo() const { return mNodeInfo; }
		MainWindow* mainWindow() const { return mMainWindow; }
		
	protected:
		virtual void paintEvent(QPaintEvent*) override;
		virtual void mousePressEvent(QMouseEvent*) override;
		virtual void mouseMoveEvent(QMouseEvent*) override;
		virtual void mouseReleaseEvent(QMouseEvent*) override;
		
	protected Q_SLOTS:
		void loadStyleSheet();
		
	protected:
		NodeInfo mNodeInfo;
		bool mIsMouseDown;
		MainWindow* mMainWindow;
	};
	
	/// A WidgetNodePrototype that is being dragged and dropped onto the patch area
	class WidgetNodePrototypeBeingDragged : public WidgetNodePrototype
	{
	public:
		WidgetNodePrototypeBeingDragged(WidgetNodePrototype const* prototypeToDrag, QMouseEvent* mousePressEvent);
		virtual void mousePressEvent(QMouseEvent*) override;
		
	protected:
		virtual void mouseMoveEvent(QMouseEvent*) override;
		virtual void mouseReleaseEvent(QMouseEvent*) override;
		
	private:
		QPoint mDragOffset;
	};
}