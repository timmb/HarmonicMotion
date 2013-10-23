//
//  NodeRendererGlWidget.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once
#include <QGLWidget>
#include <QTimer>

namespace hm
{
	class NodeRenderer;
	class NodeRendererGlWidget : public QGLWidget
	{
	public:
		NodeRendererGlWidget(QWidget* parent = nullptr);
		
		/// Sets the Renderer node that this widget draws
		void setRenderer(std::shared_ptr<NodeRenderer> renderer);
		
	protected:
		virtual QSize sizeHint() const override;
		virtual void initializeGL() override;
		virtual void resizeGL(int width, int height) override;
		virtual void paintGL() override;

	private:
		int mMaxFrameRate;
		std::shared_ptr<NodeRenderer> mRenderer;
		QTimer* mRedrawTimer;
	};
}