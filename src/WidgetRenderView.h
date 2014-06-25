#pragma once

#include "WidgetNode.h"
#include <QGLWidget>
#include <QTimer>


namespace hm
{
	class NodeRenderer;
	class GlWidget;
	
	class WidgetRenderView : public QWidget
	{
	public:
		WidgetRenderView(std::shared_ptr<NodeRenderer> node, QWidget* parent);
		virtual ~WidgetRenderView();
		
		NodePtr node() const { return mNode; }
		void setNode(NodePtr node) { mNode = node; }
		
	private:
		NodePtr mNode;
	};
	
	
	class WidgetRenderViewGl : public QGLWidget
	{
		Q_OBJECT
		
	public:
		WidgetRenderViewGl(std::shared_ptr<NodeRenderer> node, QWidget* parent);
		virtual ~WidgetRenderViewGl();
		
		virtual void initializeGL() override;
		virtual void paintGL() override;
		virtual QSize sizeHint() const override;
		
		protected Q_SLOTS:
		void timerCallback();
		
	private:
		std::shared_ptr<NodeRenderer> mNode;
		QTimer* mTimer;
		int mMaxFrameRate;
	};
}


