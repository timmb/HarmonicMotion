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

namespace hm
{
	class WidgetNode : public QWidget
	{
		Q_OBJECT;
	public:
		WidgetNode(NodePtr node, QWidget* parent=nullptr);
		
	private:
		NodePtr mNode;
	};
}