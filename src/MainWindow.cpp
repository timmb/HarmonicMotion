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

MainWindow::MainWindow(PipelinePtr pipeline)
: mConsole(nullptr)
, mLayout(nullptr)
, mPatchArea(new WidgetPatchArea(pipeline))
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
	bool success(true);
	
	success = connect(mPatchArea, SIGNAL(newInfoPanelText(QString)), this, SLOT(provideInfoPanelText(QString)));
	assert(success);
	
	QScrollArea* patchScrollArea = new QScrollArea(this);
	patchScrollArea->setWidget(mPatchArea);
	patchScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	patchScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	
	setCentralWidget(patchScrollArea);
	addDockWidget(Qt::LeftDockWidgetArea, new WidgetNodeList(this, this));
	
	QDockWidget* infoPanelDock = new QDockWidget("Info", this);
	QPlainTextEdit* infoPanel = new QPlainTextEdit(infoPanelDock);
	infoPanelDock->setWidget(infoPanel);
	addDockWidget(Qt::BottomDockWidgetArea, infoPanelDock);
	
	success = connect(this, SIGNAL(newInfoPanelText(QString)), infoPanel, SLOT(setPlainText(QString)));
	assert(success);
}


void MainWindow::provideInfoPanelText(QString text)
{
	Q_EMIT newInfoPanelText(text);
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