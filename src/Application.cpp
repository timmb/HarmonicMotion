//
//  Application.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "Application.h"
#include <QTimer>

using namespace hm;

Application::Application(int argc, char** argv)
: QApplication(argc, argv)
, mAccum(nullptr)
{
	NodePtr kinect = NodePtr(new GeneratorSineWave);
	mNodes << kinect;
	mPipeline.addNode(kinect);
	
	mAccum = std::shared_ptr<NodeAccumulator>(new NodeAccumulator);
	kinect->outlet(0)->connect(mAccum->inlet(0));
	mNodes << mAccum;
	mPipeline.addNode(mAccum);
	
	auto timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(checkPipeline()));
	timer->setInterval(10);
	timer->start();
	
	mPipeline.start();
}

void Application::checkPipeline()
{
	while (!mAccum->isEmpty())
	{
		Data data = mAccum->next();
		Q_EMIT newConsoleMessage(QString::fromStdString(data.toString()));
		std::cout << "emit new console message "<<data<<std::endl;
	}
}