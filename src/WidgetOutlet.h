//
//  WidgetOutlet.h
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
	class WidgetOutlet : public WidgetLet
	{
		Q_OBJECT
	public:
		WidgetOutlet(OutletPtr outlet, class WidgetNode* parent);
        
        OutletPtr outlet() const { return mOutlet; }
        
    protected:
        virtual void mousePressEvent(class QMouseEvent*) override;
		
	protected Q_SLOTS:
		void loadStyleSheet();
		
	private:
		OutletPtr mOutlet;
    };
}