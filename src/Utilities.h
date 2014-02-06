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
	QString str(std::string utf8String);
}