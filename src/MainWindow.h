//
//  MainWindow.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//
#pragma once

#include <QMainWindow>
#include <QTextEdit>

namespace hm
{
	class MainWindow : public QMainWindow
	{
		Q_OBJECT;
	public:
		MainWindow();
		
	private:
		QTextEdit* mConsole;
	};
}