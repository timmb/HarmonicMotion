#include "WidgetConsoleView.h"
#include "Node.h"
#include "Utilities.h"
#include <QScrollBar>

using namespace hm;

WidgetConsoleView::WidgetConsoleView(NodePtr node, QWidget* parent)
: QPlainTextEdit(parent)
, mNode(node)
, mConsoleListenerHandle(-42)
{
	Q_ASSERT(node != nullptr);
	mConsoleListenerHandle = mNode->addConsoleListener([&](std::string const& s) {
		Q_EMIT sig_newText(str(s));
	});
	setReadOnly(true);
	for (auto& p : mNode->parameters())
	{
		if (p->name() == "Buffer size" && p->type()==BaseParameter::INT)
		{
			auto param = std::dynamic_pointer_cast<Parameter<int>>(p);
			Q_ASSERT(param != nullptr);
			if (param != nullptr)
			{
				auto callback = [this, param]() { callbackBufferSizeChanged(param->lastValue()); };
				param->addNewExternalValueCallback(callback);
				callback();
			}
		}
		else if (p->name() == "Word wrap")
		{
			auto param = std::dynamic_pointer_cast<Parameter<bool>>(p);
			Q_ASSERT(param != nullptr);
			if (param != nullptr)
			{
				auto callback = [this, param]() { callbackWordWrapChanged(param->lastValue()); };
				param->addNewExternalValueCallback(callback);
				callback();
			}
		}
	}
	if (mNode->isConsoleShownByDefault())
	{
		show();
	}
	else
	{
		hide();
	}

	BOOST_VERIFY(connect(this, SIGNAL(sig_newText(QString)), this, SLOT(slot_newText(QString)), Qt::QueuedConnection));
}

WidgetConsoleView::~WidgetConsoleView()
{
	mNode->removeConsoleListener(mConsoleListenerHandle);
}

void WidgetConsoleView::slot_newText(QString text)
{
	insertPlainText(text);
	QScrollBar* scrollBar = verticalScrollBar();
	scrollBar->setValue(scrollBar->maximum());
}

void WidgetConsoleView::callbackBufferSizeChanged(int newSize)
{
	newSize = std::max(1, std::min(25000, newSize));
	setMaximumBlockCount(newSize);
}

void WidgetConsoleView::callbackWordWrapChanged(bool newWordWrap)
{
	setWordWrapMode(newWordWrap ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap);
}