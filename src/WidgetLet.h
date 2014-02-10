//
//  WidgetLet.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 10/02/2014.
//
//

#pragma once
#include <QLabel>

namespace hm
{
	/// Base class for inlet and outlet widgets
	class WidgetLet : public QLabel
	{
		Q_OBJECT
	public:
		WidgetLet(QWidget* parent=nullptr);
	};
}