//
//  WidgetNode.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 03/02/2014.
//
//

#pragma once
#include <QWidget>
#include "Common.h"
#include <QPaintEvent>

namespace hm
{
	class WidgetNode : public QWidget
	{
		Q_OBJECT;
	public:
		WidgetNode(NodePtr node, QWidget* parent=nullptr);
		
	protected:
		/// Necessary to reimplement to make stylesheet work
		/// see http://stackoverflow.com/questions/7276330/qt-stylesheet-for-custom-widget
		void paintEvent(QPaintEvent*);
		
	protected Q_SLOTS:
		void reloadStyleSheet();
		

		
	private:
		NodePtr mNode;
	};
}