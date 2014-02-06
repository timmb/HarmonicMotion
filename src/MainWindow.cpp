//
//  MainWindow.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "MainWindow.h"
#include "NodeRendererGlWidget.h"
#include <QtWidgets>
#include "WidgetNode.h"
#include "WidgetPatchArea.h"
#include "WidgetNodeList.h"

using namespace hm;

MainWindow::MainWindow()
: mConsole(nullptr)
, mLayout(nullptr)
, mPatchArea(new WidgetPatchArea)
{
//	mLayout = new QVBoxLayout;
//	QWidget* w = new QWidget(this);
//	setCentralWidget(w);
//	w->setLayout(mLayout);
		
//	mConsole = new QPlainTextEdit(w);
//	mConsole->setMaximumBlockCount(500);
//	mConsole->setReadOnly(true);
//	mConsole->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

//	mLayout->addWidget(mConsole);
//	mLayout->addWidget(mPatchArea);
	
	setCentralWidget(mPatchArea);
	addDockWidget(Qt::LeftDockWidgetArea, new WidgetNodeList(this, this));
	
}

void MainWindow::addNode(NodePtr node)
{
	new WidgetNode(node, mPatchArea);
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