//
//  WidgetLet.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 10/02/2014.
//
//

#include "WidgetLet.h"
#include "WidgetNode.h"
#include <cassert>
#include "WidgetPatchArea.h"
#include <QCoreApplication>

using namespace hm;

WidgetLet::WidgetLet(LetPtr let, WidgetNode* parent)
: QLabel(parent)
, mParent(parent)
, mLet(let)
{
	assert(let != nullptr);
    assert(mParent != nullptr);
	// we recreate the help description as rich text
	
	std::string helpText = "<b>"+let->className()+" "
	+std::to_string(let->index()+1)+": "+let->name()+"</b> (<i>";
	for (Type type: listOfTypes())
	{
		if (let->types() & type)
		{
			helpText += std::to_string(type) + ", ";
		}
	}
	// Check if we need to remove final comma
	if (helpText.back() != '(')
	{
		assert(helpText.size()>=2);
		assert(helpText.substr(helpText.size()-2, helpText.size()) == ", ");
		helpText.resize(helpText.size() - 2);
	}
	helpText += "</i>)\n"+let->description();

	setToolTip(str(helpText));
}

WidgetLet::~WidgetLet()
{
	hm_debug("WidgetLet destructor");
}

QPoint WidgetLet::connectionPoint() const
{
    return mapTo(mParent->patchArea(), QPoint(width()/2, height()/2));
}

void WidgetLet::moveEvent(QMoveEvent*)
{
    Q_EMIT positionChanged();
}

void WidgetLet::resizeEvent(QResizeEvent*)
{
    Q_EMIT positionChanged();
}


void WidgetLet::mousePressEvent(QMouseEvent* event)
{
    if (event->button()==Qt::LeftButton)
    {
        // forward to the patch area to potentially start new patchcord
		QPoint posRelativeToPatchArea = mapTo(node()->patchArea(), event->pos());
		node()->patchArea()->mousePressEventFromWidgetLet(this, posRelativeToPatchArea);
		event->accept();
    }
}

