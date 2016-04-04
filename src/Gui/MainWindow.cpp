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
#include "WidgetConsoleView.h"

using namespace hm;

MainWindow::MainWindow(PipelinePtr pipeline)
: mConsole(nullptr)
, mLayout(nullptr)
, mPatchArea(nullptr)
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


	mPatchScrollArea = new QScrollArea(this);
	mPatchArea = new WidgetPatchArea(pipeline, mPatchScrollArea);
	mPatchScrollArea->setWidget(mPatchArea);
	mPatchScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	mPatchScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	setCentralWidget(mPatchScrollArea);
	WidgetNodeList* widgetNodeList = new WidgetNodeList(this, this);
	addDockWidget(Qt::LeftDockWidgetArea, widgetNodeList);

	QDockWidget* infoPanelDock = new QDockWidget("Info", this);
	infoPanelDock->setObjectName("dockInfo");
	QPlainTextEdit* infoPanel = new QPlainTextEdit(infoPanelDock);
	infoPanelDock->setWidget(infoPanel);
	addDockWidget(Qt::BottomDockWidgetArea, infoPanelDock);

	BOOST_VERIFY(connect(mPatchArea, SIGNAL(newInfoPanelText(QString)), this, SLOT(provideInfoPanelText(QString))));



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
	QAction* actionResetView = new QAction("Reset view", this);
	QAction* actionRecompileShaders = new QAction("Recompile shaders", this);
	actionRecompileShaders->setShortcut(QKeySequence(Qt::Key_F2));


	QMenu* menuFile = new QMenu("&File", this);
	menuFile->addAction(actionNew);
	menuFile->addAction(actionOpen);
	menuFile->addAction(actionSave);
	menuFile->addAction(actionSaveAs);
	menuFile->addSeparator();
	menuFile->addAction(actionQuit);
	mRecentFilesSeparator = menuFile->addSeparator();
	for (int i = 0; i < mNumRecentFiles; i++)
	{
		QAction* a = new QAction(this);
		a->setProperty("hm_recentFileIndex", i);
		menuFile->addAction(a);
		mRecentFilesActions << a;
	}
	updateRecentFiles();

	QMenu* menuDebug = new QMenu("&Debug", this);
	menuDebug->addAction(actionPrintNodeNames);
	menuDebug->addAction(actionCheckDatatypeInvariant);
	menuDebug->addAction(actionPrintWidgets);
	menuDebug->addAction(actionResetView);
	menuDebug->addAction(actionRecompileShaders);

	mMenuWindow = new QMenu("&Window", this);
	mMenuWindow->addAction(widgetNodeList->toggleViewAction());
	mMenuWindow->addAction(infoPanelDock->toggleViewAction());

	QMenuBar* menuBar = new QMenuBar(this);
	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuDebug);
	menuBar->addMenu(mMenuWindow);

	setMenuBar(menuBar);

	BOOST_VERIFY(connect(this, SIGNAL(sig_restoreState(QByteArray)), this, SLOT(slot_restoreState(QByteArray)), Qt::QueuedConnection));

	BOOST_VERIFY(connect(this, SIGNAL(newInfoPanelText(QString)), infoPanel, SLOT(setPlainText(QString))));
	BOOST_VERIFY(connect(mPatchArea, SIGNAL(nodeRendererAdded(NodeRendererPtr)), this, SLOT(addRenderView(NodeRendererPtr))));
	BOOST_VERIFY(connect(mPatchArea, SIGNAL(nodeRendererRemoved(NodeRendererPtr)), this, SLOT(removeRenderView(NodeRendererPtr))));
	BOOST_VERIFY(connect(mPatchArea, SIGNAL(nodeConsoleAdded(NodePtr)), this, SLOT(addConsoleView(NodePtr))));
	BOOST_VERIFY(connect(mPatchArea, SIGNAL(nodeConsoleRemoved(NodePtr)), this, SLOT(removeConsoleView(NodePtr))));


	BOOST_VERIFY(connect(actionNew, SIGNAL(triggered()), this, SLOT(actionNew())));
	BOOST_VERIFY(connect(actionSave, SIGNAL(triggered()), this, SLOT(actionSave())));
	BOOST_VERIFY(connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(actionSaveAs())));
	BOOST_VERIFY(connect(actionOpen, SIGNAL(triggered()), this, SLOT(actionOpen())));
	BOOST_VERIFY(connect(actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit())));
	for (QAction* a : mRecentFilesActions)
	{
		BOOST_VERIFY(connect(a, SIGNAL(triggered()), this, SLOT(actionOpenRecentFile())));
	}

	BOOST_VERIFY(connect(actionPrintNodeNames, SIGNAL(triggered()), this, SLOT(actionPrintNodeNames())));
	BOOST_VERIFY(connect(actionCheckDatatypeInvariant, SIGNAL(triggered()), this, SLOT(actionCheckDatatypeInvariant())));
	BOOST_VERIFY(connect(actionPrintWidgets, SIGNAL(triggered()), this, SLOT(actionPrintWidgets())));
	BOOST_VERIFY(connect(actionResetView, SIGNAL(triggered()), this, SLOT(actionResetView())));
	BOOST_VERIFY(connect(actionRecompileShaders, SIGNAL(triggered()), this, SLOT(actionRecompileShaders())));

	QSettings settings;
	if (settings.contains("MainWindow/pos"))
	{
		move(settings.value("MainWindow/pos", QPoint(pos())).toPoint());
	}
	if (settings.contains("MainWindow/size"))
	{
		resize(settings.value("MainWindow/size", QSize(size())).toSize());
	}
}


MainWindow::~MainWindow()
{
	hm_debug("MainWindow destructor.");
	//	// This is a bad hack at the moment, but for some reason the destructor
	//	// of WidgetRenderView causes a crash regarding memory corruption.
	//	// Until this is fixed, we don't destroy them properly.
	//	for (auto pair: mActiveRenderViews)
	//	{
	//		pair.second->setParent(nullptr);
	//		pair.second->setNode(nullptr);
	//	}
	//	for (auto w: mInactiveRenderViews)
	//	{
	//		w->setParent(nullptr);
	//		w->setNode(nullptr);
	//	}
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
		mPatchArea->markClean();
	}
}


void MainWindow::actionSaveAs()
{
	saveAs();
}

bool MainWindow::saveAs()
{
	// TODO: Remember open file.
	QString initialDir = QSettings().value("MainWindow/lastSaveDirectory").toString();
	QString filename = QFileDialog::getSaveFileName(this, "Save pipeline...", initialDir, "Pipeline JSON files (*.json)");
	mOpenedFile = filename;
	if (!mOpenedFile.isEmpty())
	{
		// Remember directory for future
		QSettings().setValue("MainWindow/lastSaveDirectory", QFileInfo(mOpenedFile).absoluteDir().absolutePath());
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
		// Save gui settings
		root["windowState"] = std::string(saveState().toBase64().data());
		QString json = str(root.toStyledString());
		QSaveFile out(mOpenedFile);
		out.open(QIODevice::WriteOnly | QIODevice::Text);
		out.write(json.toUtf8());
		bool ok = out.commit();
		if (ok)
		{
			mPatchArea->markClean();
			updateRecentFiles(mOpenedFile);
		}
		else
		{
			QMessageBox::critical(this, "Error saving file", "An error occurred while attempting to save to " + mOpenedFile + ". The pipeline has not been saved.");
		}
		return ok;
		// TODO: Update patch area to show this open file.
	}
}


void MainWindow::actionOpen()
{
	if (confirmDestroyWithUser())
	{
		QString initialDir = QSettings().value("MainWindow/lastSaveDirectory").toString();
		QString filename = QFileDialog::getOpenFileName(this, "Open pipeline...", initialDir, "Pipeline JSON files (*.json)");
		if (!filename.isEmpty())
		{
			QSettings().setValue("MainWindow/lastSaveDirectory", QFileInfo(filename).absoluteDir().absolutePath());
			openFile(filename);
		}
	}
}


bool MainWindow::openFile(QString const& filename)
{
	QFile file(filename);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QString json = QString::fromUtf8(file.readAll());
	std::vector<std::string> errors;
	bool ok = patchArea()->pipeline()->fromJsonString(json.toStdString(), errors);
	if (ok)
	{
		mOpenedFile = filename;
		patchArea()->markClean();
		updateRecentFiles(filename);
		Json::Reader reader;
		Json::Value root;
		reader.parse(json.toUtf8().data(), root, false);
		if (root["windowState"].isString())
		{
			QByteArray windowStateBase64 = QByteArray(root["windowState"].asString().c_str());
			QByteArray windowState = QByteArray::fromBase64(windowStateBase64);
			Q_EMIT sig_restoreState(windowState);
		}
	}
	else
	{
		QStringList _errors;
		for (std::string const& s : errors)
		{
			_errors << str(s);
		}
		QMessageBox::warning(this, "Failed to open " + filename + ".", _errors.join('\n'));
	}
	return ok;
}


void MainWindow::slot_restoreState(QByteArray state)
{
	restoreState(state);
}


void MainWindow::actionSave()
{
	saveOpenFile();
}


void MainWindow::actionResetView()
{
	mPatchArea->resetView();
}

void MainWindow::actionRecompileShaders()
{
	gRequestShaderRecompile = elapsedTime();
}

void MainWindow::actionOpenRecentFile()
{
	bool ok(false);
	int index = sender()->property("hm_recentFileIndex").toInt(&ok);
	assert(ok);
	if (ok)
	{
		openRecentFile(index);
	}
}

void MainWindow::updateRecentFiles(QString const& recentFile)
{
	QSettings settings;
	QStringList recentFiles = settings.value("MainWindow/recentFiles").toStringList();
	if (!recentFile.isEmpty())
	{
		int a = recentFiles.size();
		recentFiles.removeAll(recentFile);
		int b = recentFiles.size();
		recentFiles.push_front(recentFile);
		while (recentFiles.size() > mNumRecentFiles)
		{
			recentFiles.pop_back();
		}
		settings.setValue("MainWindow/recentFiles", recentFiles);
	}
	assert(mRecentFilesActions.size() == mNumRecentFiles);
	mRecentFilesSeparator->setVisible(!recentFiles.empty());
	for (int i = 0; i < recentFiles.size(); i++)
	{
		mRecentFilesActions.at(i)->setText(QString("&%1) %2").arg(i + 1).arg(QFileInfo(recentFiles.at(i)).baseName()));
		mRecentFilesActions.at(i)->setEnabled(QFileInfo(recentFiles.at(i)).exists());
		mRecentFilesActions.at(i)->setVisible(true);
	}
	for (int i = recentFiles.size(); i < mNumRecentFiles; i++)
	{
		mRecentFilesActions.at(i)->setVisible(false);
	}
}

void MainWindow::openRecentFile(int index)
{
	if (confirmDestroyWithUser())
	{
		QStringList recentFiles = QSettings().value("MainWindow/recentFiles").toStringList();
		if (index < recentFiles.size())
		{
			openFile(recentFiles.at(index));
		}
	}
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
	QSettings().setValue("MainWindow/size", event->size());
}

void MainWindow::moveEvent(QMoveEvent* event)
{
	QSettings().setValue("MainWindow/pos", event->pos());
}


void MainWindow::actionPrintNodeNames()
{
	std::vector<std::string> names = mPatchArea->pipeline()->nodeNames();
	QList<QString> qNames;
	for (std::string n : names)
	{
		qNames << str(n);
	}
	qDebug() << "Node names in use:" << qNames;
}


void MainWindow::actionCheckDatatypeInvariant()
{
	qDebug() << "Datatype invariant: " << mPatchArea->datatypeInvariant();
}

void MainWindow::actionPrintWidgets()
{
	mPatchArea->printWidgets();
}

void MainWindow::addRenderView(NodeRendererPtr node)
{
	//qDebug() << "****** add Render View" << node.get();
	QDockWidget* dock = new QDockWidget(str(node->name()), this);
	dock->setObjectName("renderer_" + str(node->name()));
	WidgetRenderView* view = new WidgetRenderView(node, dock);
	mRenderViews.push_back(RenderWidgetPair(dock, view));
	dock->setWidget(view);
	addDockWidget(Qt::BottomDockWidgetArea, dock);
	mMenuWindow->addAction(dock->toggleViewAction());
}

void MainWindow::addConsoleView(NodePtr node)
{
	QDockWidget* dock = new QDockWidget(str(node->name() + " console"), this);
	dock->setObjectName("console_" + str(node->name()));
	WidgetConsoleView* view = new WidgetConsoleView(node, dock);
	mConsoleViews.push_back(ConsoleWidgetPair(dock, view));
	dock->setWidget(view);
	addDockWidget(Qt::BottomDockWidgetArea, dock);
	mMenuWindow->addAction(dock->toggleViewAction());
}

void MainWindow::removeRenderView(NodeRendererPtr node)
{
	//qDebug() << "****** remove Render View" << node.get();
	for (int i = 0; i < mRenderViews.size(); i++)
	{
		RenderWidgetPair pair = mRenderViews[i];
		QDockWidget* dock = pair.first;
		WidgetRenderView* view = pair.second;
		if (view->node() == node)
		{
			RenderWidgetPair widgets = mRenderViews.takeAt(i);
			mMenuWindow->removeAction(widgets.first->toggleViewAction());
			removeDockWidget(widgets.first);
			delete dock;
			return;
		}
	}
	assert(false);
}

void MainWindow::removeConsoleView(NodePtr node)
{
	for (int i = 0; i < mConsoleViews.size(); i++)
	{
		ConsoleWidgetPair pair = mConsoleViews[i];
		QDockWidget* dock = pair.first;
		WidgetConsoleView* view = pair.second;
		if (view->node() == node)
		{
			ConsoleWidgetPair widgets = mConsoleViews.takeAt(i);
			mMenuWindow->removeAction(widgets.first->toggleViewAction());
			removeDockWidget(widgets.first);
			delete dock;
			return;
		}
	}
	assert(false);
}


