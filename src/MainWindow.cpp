//
//  MainWindow.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "MainWindow.h"
#include <QtGui>
using namespace hm;

MainWindow::MainWindow()
: mConsole(nullptr)
{
//	QLayout* layout = new QVBoxLayout;
//	setLayout(layout);
	mConsole = new QPlainTextEdit(this);
	setCentralWidget(mConsole);
	
	mConsole->setMaximumBlockCount(500);
	mConsole->setReadOnly(true);
	
}

void MainWindow::newConsoleMessage(QString message)
{
	mConsole->appendPlainText(message);
}