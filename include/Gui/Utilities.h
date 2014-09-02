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
#include <QVector2D>
#include <memory>

namespace hm
{
	/// Convert a std::string to a QString assuming UTF8 encoding
	QString str(std::string const& utf8String);
	
	/// Unpolish and repolish the style of a widget to force it to update
	/// its appearance (based on its CSS) after a property has changed
	void repolish(class QWidget* widget);
	
	/// Calculate the shortest distance from \p point to line segments (\p lineStart, \p lineEnd)
	float distanceFromPointToLine(QVector2D const& lineStart, QVector2D const& lineEnd, QVector2D const& point);
	
	/// \return true if \p potentialAncestor is an ancestor (direct/indirect parent) of /p potentialChild
	bool isAncestor(QObject* potentialAncestor, QObject* potentialChild);
	
	class NodeRenderer;
	typedef std::shared_ptr<NodeRenderer> NodeRendererPtr;
}

#define hm_print(variable) qDebug() << #variable<<": "<<variable;
