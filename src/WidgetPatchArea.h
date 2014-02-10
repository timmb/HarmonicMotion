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
		
	public Q_SLOTS:
		/// Request the patch area update its size. Call this if a contained
		/// moves or is resized to ensure the scroll area
		/// encapsulating this widget updates
		void updateSize();
		
	protected:
		virtual QSize sizeHint() const override;
	};
}