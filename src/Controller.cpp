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

using namespace hm;

Controller::Controller(QObject* parent)
: QObject()
, mMainWindow(new MainWindow)
, mAccum(nullptr)
, mRedrawIsRequired(false)
{
	NodePtr kinect = NodePtr(new NodeKinect);
	mNodes << kinect;
	mPipeline.addNode(kinect);
	
	mAccum = std::shared_ptr<NodeAccumulator>(new NodeAccumulator);
//	kinect->outlet(0)->connect(mAccum->inlet(0));
	mNodes << mAccum;
	mPipeline.addNode(mAccum);

	NodeRenderer::Ptr renderer(new NodeRenderer);
	mPipeline.addNode(renderer);
	kinect->outlet(0)->connect(renderer->inlet(0));
	
	bool success = connect(this, SIGNAL(newConsoleMessage(QString)), mMainWindow, SLOT(newConsoleMessage(QString)));
	assert(success);
	
	NodeRendererGlWidget* w = mMainWindow->createRendererWidget();
	w->setRenderer(renderer);
	
	NodePtr osc(new NodeOscOut);
	kinect->outlet(0)->connect(osc->inlet(0));
	mPipeline.addNode(osc);
	
	
	auto timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(checkPipeline()));
	timer->setInterval(10);
	timer->start();
	
	mMainWindow->show();
	mPipeline.start();
}

void Controller::checkPipeline()
{
	while (!mAccum->isEmpty())
	{
		Data data = mAccum->next();
		Q_EMIT newConsoleMessage(QString::fromStdString(data.toString()));
//		std::cout << "emit new console message "<<data<<std::endl;
	}
}