//
//  WidgetInlet.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 10/02/2014.
//
//

#pragma once

#include "Common.h"
#include "WidgetLet.h"
#include <QMouseEvent>

namespace hm
{
	class WidgetInlet : public WidgetLet
	{
		Q_OBJECT
	public:
		WidgetInlet(InletPtr inlet, WidgetNode* parent);
        InletPtr inlet() const { return mInlet; }
        		
	protected Q_SLOTS:
		void loadStyleSheet();
		
	private:
		InletPtr mInlet;
		WidgetNode* mParent;
	};
	
}