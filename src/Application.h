//
//  Application.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once

#include <QApplication>
#include "HarmonicMotion.h"

namespace hm
{
	class Application : public QApplication
	{
		Q_OBJECT;
		
	public:
		Application(int argc, char** argv);
		
	Q_SIGNALS:
		void newConsoleMessage(QString);
		
	private:
		Pipeline mPipeline;
	};
}