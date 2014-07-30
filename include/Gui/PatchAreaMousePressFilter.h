#pragma once
#include <QObject>

class QEvent;

namespace hm
{
	class WidgetPatchArea;

	/// Event filter that intercepts mouse presses and passes them
	/// to the WidgetPatchArea to check if they should be intercepted
	/// and used to select a patch cord instead.
	class PatchAreaMousePressFilter : public QObject
	{
	public:
		PatchAreaMousePressFilter(WidgetPatchArea* parent);
		
	protected:
		bool eventFilter(QObject* object, QEvent* event);
		
	private:
		WidgetPatchArea* mPatchArea;
	};
}