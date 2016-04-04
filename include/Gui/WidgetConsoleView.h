#pragma once
#include <QWidget>
#include <QPlainTextEdit>
#include "Common.h"

namespace hm
{
	class Node;

	class WidgetConsoleView : public QPlainTextEdit
	{
		Q_OBJECT

	public:
		WidgetConsoleView(NodePtr node, QWidget* parent);
		virtual ~WidgetConsoleView();

		NodePtr node() const { return mNode; }

		Q_SIGNALS:
		/// Use signal/slot for thread safety when text arrives
		void sig_newText(QString);

		protected Q_SLOTS:
		void slot_newText(QString);

	private:
		void callbackBufferSizeChanged(int newSize);
		void callbackWordWrapChanged(bool newWordWrap);

		NodePtr mNode;
		int mConsoleListenerHandle;
	};
}