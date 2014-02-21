//
//  MainWindow.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//
#pragma once

#include <QMainWindow>
#include <QLayout>
#include "Common.h"

class QPlainTextEdit;

namespace hm
{
	class NodeRendererGlWidget;
	class WidgetPatchArea;
	
	class MainWindow : public QMainWindow
	{
		Q_OBJECT;
		
	public:
		/// \param pipeline will be created if null.
		MainWindow(PipelinePtr pipeline=nullptr);
		NodeRendererGlWidget* createRendererWidget();
		
		template <typename WidgetType>
		void addWidget(WidgetType* widget);
		
		WidgetPatchArea* patchArea() const { return mPatchArea; }
		
	public Q_SLOTS:
		void newConsoleMessage(QString);
		
	private:
		QPlainTextEdit* mConsole;
		QLayout* mLayout;
		QVector<NodeRendererGlWidget*> mScenes;
		WidgetPatchArea* mPatchArea;
	};
	
	
	template <typename WidgetType>
	void MainWindow::addWidget(WidgetType* widget)
	{
		mLayout->addWidget(widget);
	}
}