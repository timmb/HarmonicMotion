//
//  Application.h
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
	class Application : public QApplication
	{
		Q_OBJECT;
		
	public:
		Application(int argc, char** argv);
		
	public Q_SLOTS:
		void checkPipeline();
		
	Q_SIGNALS:
		void newConsoleMessage(QString);
	
	private:
		Pipeline mPipeline;
		std::shared_ptr<NodeAccumulator> mAccum;
		QList<NodePtr> mNodes;
	};
}