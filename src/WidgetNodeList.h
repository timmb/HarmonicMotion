//
//  WidgetNodeList.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 06/02/2014.
//
//

#pragma once
#include <QDockWidget>

namespace hm
{
	class MainWindow;
	
	class WidgetNodeList : public QDockWidget
	{
		Q_OBJECT
	public:
		WidgetNodeList(MainWindow* mainWindow, QWidget* parent=nullptr);
		
	protected:
		virtual void paintEvent(QPaintEvent*) override;

	protected Q_SLOTS:
		void loadStyleSheet();
		
	private:
	};
}