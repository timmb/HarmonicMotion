#include "MouseGrabber.h"
#include <QApplication>
#include <cassert>

using namespace hm;

MouseGrabber::MouseGrabber(QWidget* parent)
: QObject(parent)
, mIsEnabled(false)
, mParent(parent)
{
	assert(mParent != nullptr);
}

MouseGrabber::~MouseGrabber()
{
	setEnabled(false);
}

void MouseGrabber::setEnabled(bool isEnabled)
{
	if (isEnabled && !mIsEnabled)
	{
		qApp->installEventFilter(this);
	}
	else if (!isEnabled && mIsEnabled)
	{
		qApp->removeEventFilter(this);
	}
	mIsEnabled = isEnabled;
}

bool MouseGrabber::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::MouseMove)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		QPoint localPos = mParent->mapFromGlobal(mouseEvent->globalPos());
		QMouseEvent* newEvent = new QMouseEvent(mouseEvent->type(),
												localPos,
												mouseEvent->button(),
												mouseEvent->buttons(),
												mouseEvent->modifiers());
		qApp->postEvent(mParent, newEvent);
	}
	return false;
}
