//
//  WidgetNodePrototype.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 06/02/2014.
//
//

#include "WidgetNodePrototype.h"
//#include "FactoryNode.h"
#include "Common.h"
#include <QStyleOption>
#include <QPainter>
#include <QFile>
#include <QLabel>
#include <QBoxLayout>
#include <QMouseEvent>
#include "Utilities.h"
#include "MainWindow.h"
#include "WidgetPatchArea.h"
#include "WidgetNode.h"
#include <QApplication>
#include <QTimer>
#include <QScrollArea>

using namespace hm;

WidgetNodePrototype::WidgetNodePrototype(NodeInfo const& info, MainWindow* mainWindow, QWidget* parent)
: QWidget(parent)
, mNodeInfo(info)
, mIsMouseDown(false)
, mMainWindow(mainWindow)
{
	assert(mMainWindow != nullptr);
	setObjectName("WidgetNodePrototype");
	loadStyleSheet();
	
	QLabel* name = new QLabel(str(info.friendlyName));
	name->setObjectName("name");
	name->setWordWrap(true);
	QLabel* description = new QLabel(str(info.friendlyDescription));
	description->setObjectName("description");
	description->setWordWrap(true);
	
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(name);
	layout->addWidget(description);
	
	setLayout(layout);
	
//	auto t = new QTimer(this);
//	t->setInterval(500);
//	connect(t, SIGNAL(timeout()), this, SLOT(loadStyleSheet()));
//	t->start();
}




void WidgetNodePrototype::loadStyleSheet()
{
	//	QFile file(":/qss/WidgetNode.qss");
	QFile file("/Users/timmb/Documents/Programming/HarmonicMotion/HarmonicMotionGui/resources/qss/WidgetNodePrototype.qss");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		setStyleSheet(QString::fromUtf8(file.readAll()));
	}
	else
	{
		hm_error("Failed to load stylesheet WidgetNodePrototype.qss");
	}
}

void WidgetNodePrototype::paintEvent(QPaintEvent *)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void WidgetNodePrototype::mousePressEvent(QMouseEvent* e)
{
	qDebug() << "WidgetNodePrototype::mousePressEvent";
	e->accept();
	mIsMouseDown = true;
}

void WidgetNodePrototype::mouseMoveEvent(QMouseEvent* e)
{
	if (mIsMouseDown)
	{
		qDebug() << "WidgetNodePrototype drag";
		e->accept();
		auto drag = new WidgetNodePrototypeBeingDragged(this, e);
		drag->show();
		mIsMouseDown = false;
	}
}

void WidgetNodePrototype::mouseReleaseEvent(QMouseEvent* e)
{
	e->accept();
	mIsMouseDown = false;
}


// ----------------------------------------

WidgetNodePrototypeBeingDragged::WidgetNodePrototypeBeingDragged(WidgetNodePrototype const* prototype, QMouseEvent* mousePressEvent)
: WidgetNodePrototype(prototype->nodeInfo(), prototype->mainWindow(), (QWidget*) prototype->mainWindow())
{
	setObjectName("WidgetNodePrototypeBeingDragged");
	setGeometry(prototype->geometry());
	//TODO: Make new object apepar in the corrent place immediately
//	move(mapFromGlobal(prototype->mapToGlobal(QPoint())));
	mDragOffset = mousePressEvent->pos();
	// Grab the mouse to continue the drag that was started on the original WidgetNodePrototype
	grabMouse();
}

void WidgetNodePrototypeBeingDragged::mousePressEvent(QMouseEvent* event)
{
//	// We shouldnt receive mouse press events as this dragging widget will
//	// disappear as soon as the mouse is released
	hm_error("WidgetNodePrototypeBeingDragged received unexpected mousePressEvent");
}

void WidgetNodePrototypeBeingDragged::mouseMoveEvent(QMouseEvent* e)
{
	e->accept();
	move(mapToParent(e->pos() - mDragOffset));
}

void WidgetNodePrototypeBeingDragged::mouseReleaseEvent(QMouseEvent* e)
{
	e->accept();
	// we consider a node as dragged onto the patch area if its to the lower
	// right of the patch area origin and within the main window geometry.
//	WidgetPatchArea* patchArea = mMainWindow->patchArea();
//	bool willCreate = x() > patchArea->x() && y() > patchArea->y()
//		&& x() < mMainWindow->width() && y() < mMainWindow->height();
	bool willCreate = mMainWindow->patchScrollArea()->geometry().intersects(geometry());
	if (willCreate)
	{
		NodePtr node = FactoryNode::instance()->create(mNodeInfo.className);
		mMainWindow->patchArea()->pipeline()->addNode(node);
	}
	deleteLater();
}

