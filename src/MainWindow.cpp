//
//  MainWindow.cpp
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#include "MainWindow.h"
#include "WidgetMonitor.h"
#include <QtWidgets>
#include "WidgetNode.h"
#include "WidgetPatchArea.h"
#include "WidgetNodeList.h"
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>
#include <QSaveFile>
#include "json/json.h"
#include "Utilities.h"
#include <QStringList>
#include <QVector>
#include <set>
#include "Node.h"
#include "nodes/NodeRenderer.h"
#include "WidgetRenderView.h"

using namespace hm;

MainWindow::MainWindow(PipelinePtr pipeline)
: mConsole(nullptr)
, mLayout(nullptr)
, mPatchArea(new WidgetPatchArea(pipeline))
, mPatchScrollArea(nullptr)
{
//	mLayout = new QVBoxLayout;
//	QWidget* w = new QWidget(this);
//	setCentralWidget(w);
//	w->setLayout(mLayout);
		
//	mConsole = new QPlainTextEdit(w);
//	mConsole->setMaximumBlockCount(500);
//	mConsole->setReadOnly(true);
//	mConsole->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

//	mLayout->addWidget(mConsole);
//	mLayout->addWidget(mPatchArea);
	bool success(true);
	
	success = connect(mPatchArea, SIGNAL(newInfoPanelText(QString)), this, SLOT(provideInfoPanelText(QString)));
	assert(success);
	
	mPatchScrollArea = new QScrollArea(this);
	mPatchScrollArea->setWidget(mPatchArea);
	mPatchScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	mPatchScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	
	setCentralWidget(mPatchScrollArea);
	addDockWidget(Qt::LeftDockWidgetArea, new WidgetNodeList(this, this));
	
	QDockWidget* infoPanelDock = new QDockWidget("Info", this);
	QPlainTextEdit* infoPanel = new QPlainTextEdit(infoPanelDock);
	infoPanelDock->setWidget(infoPanel);
	addDockWidget(Qt::BottomDockWidgetArea, infoPanelDock);
	
	QAction* actionNew = new QAction("New", this);
	actionNew->setShortcut(QKeySequence::New);
	QAction* actionSave = new QAction("Save", this);
	actionSave->setShortcut(QKeySequence::Save);
	QAction* actionSaveAs = new QAction("Save as...", this);
	actionSaveAs->setShortcut(QKeySequence::SaveAs);
	QAction* actionOpen = new QAction("Open...", this);
	actionOpen->setShortcut(QKeySequence::Open);
	QAction* actionQuit = new QAction("Quit", this);
	actionQuit->setMenuRole(QAction::QuitRole);
	actionQuit->setShortcut(QKeySequence::Quit);
	
	QAction* actionPrintNodeNames = new QAction("Print &node names in use", this);
	QAction* actionCheckDatatypeInvariant = new QAction("Check datatype invariant", this);
	QAction* actionPrintWidgets = new QAction("Print widgets", this);
	
	
	QMenu* menuFile = new QMenu("&File", this);
	menuFile->addAction(actionNew);
	menuFile->addAction(actionOpen);
	menuFile->addAction(actionSave);
	menuFile->addAction(actionSaveAs);
	menuFile->addSeparator();
	menuFile->addAction(actionQuit);
	
	QMenu* menuDebug = new QMenu("&Debug", this);
	menuDebug->addAction(actionPrintNodeNames);
	menuDebug->addAction(actionCheckDatatypeInvariant);
	menuDebug->addAction(actionPrintWidgets);
	
	QMenuBar* menuBar = new QMenuBar(this);
	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuDebug);
	
	setMenuBar(menuBar);
	
	success = connect(this, SIGNAL(newInfoPanelText(QString)), infoPanel, SLOT(setPlainText(QString)));
	assert(success);
	BOOST_VERIFY(connect(mPatchArea, SIGNAL(nodeRendererAdded(NodeRendererPtr)), this, SLOT(addRenderView(NodeRendererPtr))));
	BOOST_VERIFY(connect(mPatchArea, SIGNAL(nodeRendererRemoved(NodeRendererPtr)), this, SLOT(removeRenderView(NodeRendererPtr))));
	
	
	success = connect(actionNew, SIGNAL(triggered()), this, SLOT(actionNew()));
	assert(success);
	success = connect(actionSave, SIGNAL(triggered()), this, SLOT(actionSave()));
	assert(success);
	success = connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(actionSaveAs()));
	assert(success);
	success = connect(actionOpen, SIGNAL(triggered()), this, SLOT(actionOpen()));
	assert(success);
	success = connect(actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
	
	success = connect(actionPrintNodeNames, SIGNAL(triggered()), this, SLOT(actionPrintNodeNames()));
	assert(success);
	success = connect(actionCheckDatatypeInvariant, SIGNAL(triggered()), this, SLOT(actionCheckDatatypeInvariant()));
	assert(success);
	success = connect(actionPrintWidgets, SIGNAL(triggered()), this, SLOT(actionPrintWidgets()));
	assert(success);
}


MainWindow::~MainWindow()
{
	hm_debug("MainWindow destructor.");
}


void MainWindow::provideInfoPanelText(QString text)
{
	Q_EMIT newInfoPanelText(text);
}


//NodeRendererGlWidget* MainWindow::createRendererWidget()
//{
//	auto w = new NodeRendererGlWidget(this);
//	mScenes.push_back(w);
//	mLayout->addWidget(w);
//	return w;
//}
//

void MainWindow::newConsoleMessage(QString message)
{
	mConsole->appendPlainText(message);
}


void MainWindow::actionNew()
{
	if (confirmDestroyWithUser())
	{
		mPatchArea->pipeline()->clear();
	}
}


void MainWindow::actionSaveAs()
{
	saveAs();
}

bool MainWindow::saveAs()
{
	// TODO: Remember open file.
	// TODO: Persistent store of last directory
	QString filename = QFileDialog::getSaveFileName(this, "Save pipeline...", QString(), "Pipeline JSON files (*.json)");
	mOpenedFile = filename;
	if (!mOpenedFile.isEmpty())
	{
		return saveOpenFile();
	}
	else
	{
		return false;
	}
}

bool MainWindow::saveOpenFile()
{
	if (mOpenedFile.isEmpty())
	{
		return saveAs();
	}
	else
	{
		Json::Value root;
		mPatchArea->pipeline()->toJson(root);
		QString json = str(root.toStyledString());
		QSaveFile out(mOpenedFile);
		out.open(QIODevice::WriteOnly | QIODevice::Text);
		out.write(json.toUtf8());
		bool ok = out.commit();
		if (ok)
		{
			mPatchArea->markClean();
		}
		else
		{
			QMessageBox::critical(this, "Error saving file", "An error occurred while attempting to save to "+mOpenedFile+". The pipeline has not been saved.");
		}
		return ok;
		// TODO: Update patch area to show this open file.
	}
}


void MainWindow::actionOpen()
{
	if (confirmDestroyWithUser())
	{
		QString filename = QFileDialog::getOpenFileName(this, "Open pipeline...", QString(), "Pipeline JSON files (*.json)");
		if (!filename.isEmpty())
		{
			QFile file(filename);
			file.open(QIODevice::ReadOnly | QIODevice::Text);
			QString json = QString::fromUtf8(file.readAll());
			std::vector<std::string> errors;
			bool ok = patchArea()->pipeline()->fromJsonString(json.toStdString(), errors);
			if (!ok)
			{
				QStringList _errors;
				for (std::string const& s: errors)
				{
					_errors << str(s);
				}
				QMessageBox::warning(this, "Failed to open "+filename+".", _errors.join('\n'));
			}
		}
	}
}


void MainWindow::actionSave()
{
	saveOpenFile();
}


bool MainWindow::confirmDestroyWithUser()
{
	if (mPatchArea->isDirty())
	{
		QMessageBox confirm;
		confirm.setText("The pipeline has been modified.");
		confirm.setInformativeText("Do you want to save your changes?");
		confirm.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		confirm.setDefaultButton(QMessageBox::Save);
		int ret = confirm.exec();
		bool okToContinue = false;
		switch (ret)
		{
			case QMessageBox::Save:
				okToContinue = saveOpenFile();
				break;
			case QMessageBox::Discard:
				okToContinue = true;
				break;
			case QMessageBox::Cancel:
				okToContinue = false;
				break;
			default:
				assert(false);
		}
		return okToContinue;
	}
	else
	{
		return true;
	}
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
//	hm_debug("MainWindow::resizeEvent");
}


void MainWindow::actionPrintNodeNames()
{
	std::vector<std::string> names = mPatchArea->pipeline()->nodeNames();
	QList<QString> qNames;
	for (std::string n: names)
	{
		qNames << str(n);
	}
	qDebug() << "Node names in use:" << qNames;
}


void MainWindow::actionCheckDatatypeInvariant()
{
	qDebug() << "Datatype invariant: "<<mPatchArea->datatypeInvariant();
}

void MainWindow::actionPrintWidgets()
{
	mPatchArea->printWidgets();
}

void MainWindow::addRenderView(NodeRendererPtr node)
{
	qDebug() <<"****** add Render View" << node.get();
	QDockWidget* dock = new QDockWidget(str(node->name()), this);
	WidgetRenderView* view = new WidgetRenderView(node, dock);
	mRenderViews.push_back(view);
	dock->setWidget(view);
	addDockWidget(Qt::BottomDockWidgetArea, dock);

}

void MainWindow::removeRenderView(NodeRendererPtr node)
{
	qDebug() <<"****** remove Render View" << node.get();
	for (auto it=mRenderViews.begin(); it!=mRenderViews.end(); ++it)
	{
		WidgetRenderView* view = *it;
		if (view->node() == node)
		{
			// view's parent is its containing dock widget
			delete view->parent();
			mRenderViews.erase(it);
			return;
		}
	}
	assert(false);
}


