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

namespace hm
{
	class WidgetInlet : public WidgetLet
	{
		Q_OBJECT
	public:
		WidgetInlet(InletPtr inlet, QWidget* parent);
		
	protected Q_SLOTS:
		void loadStyleSheet();
		
	private:
		InletPtr mInlet;
	};
	
}