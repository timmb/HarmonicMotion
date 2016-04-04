#pragma once

#include "WidgetNode.h"
#include <QGLWidget>
#include <QTimer>
#include "Utilities.h"


namespace hm
{
	class NodeRenderer;
	class WidgetRenderViewGl;


	class WidgetRenderView : public QWidget
	{
	public:
		WidgetRenderView(NodeRendererPtr node, QWidget* parent);
		virtual ~WidgetRenderView();

		NodeRendererPtr node() const;
		void setNode(NodeRendererPtr node);

	private:
		WidgetRenderViewGl* mGlWidget;
	};


	class WidgetRenderViewGl : public QGLWidget
	{
		Q_OBJECT

	public:
		WidgetRenderViewGl(std::shared_ptr<NodeRenderer> node, QWidget* parent);
		virtual ~WidgetRenderViewGl();

		void start() { mTimer->start(); }
		void stop() { mTimer->stop(); }

		virtual void initializeGL() override;
		virtual void paintGL() override;
		virtual QSize sizeHint() const override;

		NodeRendererPtr node() const;
		void setNode(NodeRendererPtr node);

		protected Q_SLOTS:
		void timerCallback();

	private:
		bool mForceRedraw;
		NodeRendererPtr mNode;
		QTimer* mTimer;
		int mMaxFrameRate;
	};
}


