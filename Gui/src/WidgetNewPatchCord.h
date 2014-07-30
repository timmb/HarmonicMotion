//
//  WidgetNewPatchCord.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/05/2014.
//
//

#pragma once

#include <QWidget>
#include <QLine>
class QPaintEvent;

namespace hm
{
	class WidgetPatchArea;
	class WidgetInlet;
	class WidgetOutlet;
	
	class WidgetNewPatchCord : public QWidget
	{
	public:
		WidgetNewPatchCord(WidgetInlet* inlet, WidgetPatchArea* patchArea);
		WidgetNewPatchCord(WidgetOutlet* outlet, WidgetPatchArea* patchArea);
		virtual ~WidgetNewPatchCord();
		
	protected:
		virtual void paintEvent(QPaintEvent* event) override;
		virtual void mousePressEvent(QMouseEvent* event) override;
		virtual void mouseMoveEvent(QMouseEvent* event) override;
		virtual void keyPressEvent(QKeyEvent* event) override;
		
	private:
		void init();
		
		QLine mLine;
		WidgetPatchArea* mPatchArea;
		WidgetInlet* mInlet;
		WidgetOutlet* mOutlet;
	};
}