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
	class MainWindow : public QMainWindow
	{
		Q_OBJECT;
	public:
		MainWindow();
		
	public Q_SLOTS:
		void newConsoleMessage(QString);
		
	private:
		QPlainTextEdit* mConsole;
	};
}