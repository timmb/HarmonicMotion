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
#include "NodeOscOutWidget.h"

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

	bool success = connect(this, SIGNAL(newConsoleMessage(QString)), mMainWindow, SLOT(newConsoleMessage(QString)));
	assert(success);
	
	NodeOscOut::Ptr osc(new NodeOscOut);
	kinect->outlet(0)->connect(osc->inlet(0));
	mPipeline.addNode(osc);
	mMainWindow->addWidget(new NodeOscOutWidget(osc));
	
	auto filter = NodePtr(new NodeFilter);
	kinect->outlet(0)->connect(filter->inlet(0));
	mPipeline.addNode(filter);
	
//	auto filterOut = NodePtr(new OutputConsolePrinter);
//	filter->outlet(0)->connect(filterOut->inlet(0));
//	kinect->outlet(0)->connect(filterOut->inlet(0));
//	mPipeline.addNode(filterOut);
	
	auto renderer0 = NodeRenderer::Ptr(new NodeRenderer);
	mPipeline.addNode(renderer0);
	
	auto renderer1 = NodeRenderer::Ptr(new NodeRenderer);
	mPipeline.addNode(renderer1);
	
	filter->outlet(0)->connect(renderer0->inlet(0));
	kinect->outlet(0)->connect(renderer1->inlet(0));
	
	NodeRendererGlWidget* w = mMainWindow->createRendererWidget();
	w->setRenderer(renderer0);
	auto w2 = mMainWindow->createRendererWidget();
	w2->setRenderer(renderer1);
	
	
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