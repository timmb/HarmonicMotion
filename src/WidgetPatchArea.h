//
//  WidgetPatchArea.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 05/02/2014.
//
//

#pragma once

#include <QWidget>

namespace hm
{
	class WidgetPatchArea : public QWidget
	{
		Q_OBJECT
	public:
		WidgetPatchArea(QWidget* parent=nullptr);
		
	protected:
		virtual QSize sizeHint() const override;
	};
}