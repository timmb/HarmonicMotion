#pragma once
#include <QObject>
#include <QEvent>
#include <QMouseEvent>
#include <QWidget>

namespace hm
{
	class MouseGrabber : public QObject
	{
	public:
		
		MouseGrabber(QWidget* parent);
		virtual ~MouseGrabber();
		
		void setEnabled(bool isEnabled);
		bool isEnabled() const;
		
		bool eventFilter(QObject* object, QEvent* event);
		
	private:
		bool mIsEnabled;
		QWidget* mParent;
	};
}