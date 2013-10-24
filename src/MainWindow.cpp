//
//  MainWindow.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "MainWindow.h"
#include "NodeRendererGlWidget.h"
#include <QtGui>
using namespace hm;

MainWindow::MainWindow()
: mConsole(nullptr)
, mLayout(nullptr)
{
	mLayout = new QVBoxLayout;
	QWidget* w = new QWidget(this);
	setCentralWidget(w);
	w->setLayout(mLayout);
	
	
	
	mConsole = new QPlainTextEdit(w);
	mLayout->addWidget(mConsole);
	
	mConsole->setMaximumBlockCount(500);
	mConsole->setReadOnly(true);
	
}

NodeRendererGlWidget* MainWindow::createRendererWidget()
{
	auto w = new NodeRendererGlWidget(this);
	mScenes.push_back(w);
	mLayout->addWidget(w);
	return w;
}


void MainWindow::newConsoleMessage(QString message)
{
	mConsole->appendPlainText(message);
}