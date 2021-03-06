//
//  Controller.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once
#include <QtGui>
#include "HarmonicMotion.h"


namespace hm
{
	class MainWindow;
	
	class Controller : public QObject
	{
		Q_OBJECT;
		
	public:
		Controller(QObject* parent=nullptr);
		virtual ~Controller();
		
	public Q_SLOTS:
		void checkPipeline();
		
	Q_SIGNALS:
		void newConsoleMessage(QString);
	
	private:
		MainWindow* mMainWindow;
		PipelinePtr mPipeline;
		std::shared_ptr<NodeAccumulator> mAccum;
		QList<NodePtr> mNodes;
		std::atomic<bool> mRedrawIsRequired;
	};
}