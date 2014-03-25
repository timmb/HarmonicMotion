//
//  Utilities.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 06/02/2014.
//
//
/// Common utility functions

#pragma once
#include <QString>
#include <string>
#include <cassert>
#include <QDebug>

namespace hm
{
	/// Convert a std::string to a QString assuming UTF8 encoding
	QString str(std::string const& utf8String);
	
	/// Unpolish and repolish the style of a widget to force it to update
	/// its appearance (based on its CSS) after a property has changed
	void repolish(class QWidget* widget);
}