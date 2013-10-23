//
//  MainWindow.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//
#pragma once

#include <QMainWindow>

class QPlainTextEdit;

namespace hm
{
	class NodeRendererGlWidget;
	
	class MainWindow : public QMainWindow
	{
		Q_OBJECT;
	public:
		MainWindow();
		NodeRendererGlWidget* createRendererWidget();
		
		
	public Q_SLOTS:
		void newConsoleMessage(QString);
		
	private:
		QPlainTextEdit* mConsole;
		QLayout* mLayout;
		QVector<NodeRendererGlWidget*> mScenes;
	};
}