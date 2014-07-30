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
	
	// based on http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
	float distanceFromPointToLine(QVector2D const& lineStart, QVector2D const& lineEnd, QVector2D const& point) {
		QVector2D const& v = lineStart;
		QVector2D const& w = lineEnd;
		QVector2D const& p = point;
		// Return minimum distance between line segment vw and point p
		float const l2 = (v - w).lengthSquared();  // i.e. |w-v|^2 -  avoid a sqrt
		if (l2 == 0.0) return (p-v).length();   // v == w case
		// Consider the line extending the segment, parameterized as v + t (w - v).
		// We find projection of point p onto the line.
		// It falls where t = [(p-v) . (w-v)] / |w-v|^2
		float const t = QVector2D::dotProduct(p - v, w - v) / l2;
		if (t < 0.0)
			return (p - v).length();       // Beyond the 'v' end of the segment
		else if (t > 1.0)
			return (p - w).length();  // Beyond the 'w' end of the segment
		QVector2D const projection = v + t * (w - v);  // Projection falls on the segment
		return (p - projection).length();
	}
	
	bool isAncestor(QObject* potentialAncestor, QObject* potentialChild)
	{
		return potentialAncestor!=nullptr
		&& potentialChild!=nullptr
		&& (potentialChild->parent() == potentialAncestor
			|| isAncestor(potentialAncestor, potentialChild->parent())
			);
	}
}