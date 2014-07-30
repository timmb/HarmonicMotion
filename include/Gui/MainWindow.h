//
//  MainWindow.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//
#pragma once

#include <QMainWindow>
#include <QLayout>
#include "Common.h"
#include <QSaveFile>
#include "Utilities.h"
#include <QMap>
#include "MetatypeDeclarations.h"

class QPlainTextEdit;
class QScrollArea;

namespace hm
{
	class WidgetPatchArea;
	class NodeRenderer;
	class WidgetRenderView;
	
	class MainWindow : public QMainWindow
	{
		Q_OBJECT;
		
	public:
		/// \param pipeline will be created if null.
		MainWindow(PipelinePtr pipeline=nullptr);
		virtual ~MainWindow();
//		NodeRendererGlWidget* createRendererWidget();
		
//		template <typename WidgetType>
//		void addWidget(WidgetType* widget);
		
		WidgetPatchArea* patchArea() const { return mPatchArea; }
		QScrollArea* patchScrollArea() const { return mPatchScrollArea; }
		
	public Q_SLOTS:
		void newConsoleMessage(QString);
		/// Update the info panel
		void provideInfoPanelText(QString);
		void actionNew();
		void actionSave();
		void actionSaveAs();
		void actionOpen();
		void actionResetView();
		
		void addRenderView(NodeRendererPtr node);
		void removeRenderView(NodeRendererPtr node);
		
		// Debug actions
		void actionPrintNodeNames();
		void actionCheckDatatypeInvariant();
		void actionPrintWidgets();
		
		// TODO: Implement close event and check whether we should save
		
	Q_SIGNALS:
		void newInfoPanelText(QString);
		
	protected:
		virtual void resizeEvent(QResizeEvent* event) override;
		
	private:
		/// Asks the user if they want to save current document.
		/// \return true if it's OK to carry on and destroy the document
		/// or false if the user clicked Cancel.
		bool confirmDestroyWithUser();
		/// Presents a save as dialog, returns true if the file successfully saves
		bool saveAs();
		/// Saves the current pipeline to mOpenedFile, or launches actionSaveAs
		/// if mOpenedFile does not point to a file
		bool saveOpenFile();
		
		QPlainTextEdit* mConsole;
		QLayout* mLayout;
//		QVector<NodeRendererGlWidget*> mScenes;
		WidgetPatchArea* mPatchArea;
		QScrollArea* mPatchScrollArea;
		QString mOpenedFile;
		
		typedef QPair<QDockWidget*, WidgetRenderView*> RenderWidgetPair;
		QList<RenderWidgetPair> mRenderViews;
	};
	
	
//	template <typename WidgetType>
//	void MainWindow::addWidget(WidgetType* widget)
//	{
//		mLayout->addWidget(widget);
//	}
}