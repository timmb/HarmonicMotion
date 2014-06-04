#include "PatchAreaMousePressFilter.h"
#include "WidgetPatchArea.h"
#include <QMouseEvent>
#include <QDebug>
#include "Utilities.h"

using namespace hm;

PatchAreaMousePressFilter::PatchAreaMousePressFilter(WidgetPatchArea* parent)
: QObject(parent)
, mPatchArea(parent)
{
	assert(mPatchArea != nullptr);
}


bool PatchAreaMousePressFilter::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* e = dynamic_cast<QMouseEvent*>(event);
		QWidget* widget = dynamic_cast<QWidget*>(object);
		bool patchAreaIsParent = isAncestor(mPatchArea, object) || mPatchArea == object;
		assert(patchAreaIsParent);
		assert(e);
		assert(widget);
		if (e && widget && patchAreaIsParent)
		{
			QPoint mouseRelativeToPatchArea = widget->mapTo(mPatchArea, e->pos());
			if (mPatchArea->checkToInterceptMousePress(mouseRelativeToPatchArea, e->button(), e->buttons(), e->modifiers()))
			{
				e->accept();
				return true;
			}
		}
	}
	return false;
}