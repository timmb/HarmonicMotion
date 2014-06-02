//
//  Controller.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "Controller.h"
#include "MainWindow.h"
#include "HarmonicMotion.h"
#include "NodeRendererGlWidget.h"
#include <QTimer>
#include "WidgetNode.h"
#include "FactoryNode.h"
#include "WidgetPatchArea.h"


using namespace hm;

Controller::Controller(QObject* parent)
: QObject()
, mMainWindow(new MainWindow)
, mPipeline(nullptr)
, mAccum(nullptr)
, mRedrawIsRequired(false)
{
	mPipeline = mMainWindow->patchArea()->pipeline();
	NodePtr gen = FactoryNode::instance()->create("NodeSineWave");
	mPipeline->addNode(gen);
	
	NodePtr expr = FactoryNode::instance()->create("NodeExpression");
	mPipeline->addNode(expr);
	
	NodePtr print = FactoryNode::instance()->create("NodePrinter");
	mPipeline->addNode(print);
	
	mPipeline->connect(gen->outlet(0), expr->inlet(0));
	mPipeline->connect(expr->outlet(0), print->inlet(0));
	
	bool success = connect(this, SIGNAL(newConsoleMessage(QString)), mMainWindow, SLOT(newConsoleMessage(QString)));
	assert(success);
	
	mMainWindow->show();
	mPipeline->start();
}

void Controller::checkPipeline()
{

}