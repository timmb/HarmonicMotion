//
//  Utilities.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 06/02/2014.
//
//

#include "Utilities.h"
#include <QWidget>
#include <QStyle>

namespace hm
{
	QString str(std::string const& utf8String)
	{
		return QString::fromUtf8(utf8String.c_str());
	}
	
	void repolish(QWidget* widget)
	{
		widget->style()->unpolish(widget);
		widget->style()->polish(widget);
	}
}