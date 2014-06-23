//
//  WidgetPatchArea.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 05/02/2014.
//
//

#pragma once

#include <QWidget>
#include <QMap>
#include "Common.h"
#include <QList>
#include "Pipeline.h"
#include "PipelineListener.h"
#include "Utilities.h"
#include "MetatypeDeclarations.h"

namespace hm
{
    class WidgetPatchCord;
    class WidgetNode;
    class WidgetInlet;
    class WidgetOutlet;
	class WidgetLet;
	class WidgetNewPatchCord;
	class MouseGrabber;
	class PatchAreaMousePressFilter;
    
    
	/// WidgetPatchArea manages a Pipeline.
	class WidgetPatchArea : public QWidget
	{
		Q_OBJECT
	public:
		/// \param pipeline The pipeline that this patch area will represent.
		/// Will be created if null.
		WidgetPatchArea(PipelinePtr pipeline, QWidget* parent=nullptr);
		virtual ~WidgetPatchArea();
		
		/// \return Whether the pipeline is dirty (i.e. has unsaved data)
		bool isDirty() const { return mIsDirty; }
		
		/// Clears the dirty flag indicating that there is now no unsaved
		/// data on this pipeline.
		void markClean();
		
		/// \return A shared pointer to the pipeline represented by this patch
		/// area
		PipelinePtr pipeline() const { return mPipeline; }
        
		// MARK: Element access and querying
		/// \return The patch cord widget between \p outlet and \inlet
        /// or nullptr if they are not connected.
        WidgetPatchCord* getPatchCord(WidgetOutlet* outlet, WidgetInlet* inlet) const;
        
        /// If there is an outlet under point \p position then this will
        /// return a pointer to it. Otherwise it will return nullptr.
        WidgetOutlet* findOutlet(QPoint position) const;
        /// If there is an inlet under point \p position then this will
        /// return a pointer to it. Otherwise it will return nullptr.
        WidgetInlet* findInlet(QPoint position) const;
        
        /// \copydoc isConnectionValid(OutletPtr, InletPtr)
		bool isConnectionValid(WidgetOutlet* outlet, WidgetInlet* inlet) const;
		
		// MARK: Functions that cause modifications the underlying Pipeline model (as well as this view).
		
		// MARK: Other functions
		/// This is called by WidgetLet when it receives a mouse press
		/// event.
		/// \param position is the coordinates of the mouse relative to
		/// this WidgetPatchArea.
		void mousePressEventFromWidgetLet(WidgetLet* let, QPoint position);
		/// This is called by an event filter over all widgets to check
		/// if a mouse press event should be intercepted and used to
		/// select a patch cord instead. If it should be then the
		/// respective patch cord is selected instead.
		/// \param pos should be the position of the mouse relative to
		/// this widget.
		/// \return true if the press should be intercepted.
		bool checkToInterceptMousePress(QPoint const& pos, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);
		/// This is called by WidgetNewPatchCord when it has completed its
		/// purpose and would like to be deleted.
		void clearNewPatchCord(WidgetNewPatchCord* toBeCleared);
		/// This is called by a WidgetNode that would like to be raised
		/// to the top of the z-order stack. It will raise the widget
		/// and then re-raise all the patch cords to be above it.
		void raise(WidgetNode* w);
		
		
		// MARK: For debugging
		/// \return true if the widgets in the patch area match the
		/// underlying model
		bool datatypeInvariant() const;
		void printWidgets() const;
		
	public Q_SLOTS:
		/// Request the patch area update its size. Call this if a contained
		/// moves or is resized to ensure the scroll area
		/// encapsulating this widget updates
		void updateSize();
		/// Provide new Info Panel data to this patch area, to be forwarded
		/// on to the info panel
		void provideInfoPanelText(QString);
		void loadStyleSheet();
		/// Mark the pipeline as dirty, meaning the user will be prompted
		/// to save the next time they do something that would lose everything
		void markDirty();
		
	protected Q_SLOTS:
		// MARK: PipelineListener slots
		// These allow the widget to stay updated when the pipeline changes
		void nodeAdded(NodePtr node);
		void nodeRemoved(NodePtr node);
		void nodeParamsChanged(NodePtr node);
		void nodeCharacteristicsChanged(NodePtr node);
		void patchCordAdded(OutletPtr outlet, InletPtr inlet);
		void patchCordRemoved(OutletPtr outlet, InletPtr inlet);

        
	Q_SIGNALS:
		/// New text to be sent to the info panel
		void newInfoPanelText(QString);
		
		void nodeRendererAdded(NodeRendererPtr node);
		void nodeRendererRemoved(NodeRendererPtr node);
	
	protected:
		virtual QSize sizeHint() const override;
//		virtual void mousePressEvent(QMouseEvent*) override;
//		virtual void mouseMoveEvent(QMouseEvent*) override;
//		virtual void keyPressEvent(QKeyEvent*) override;
		virtual void focusInEvent(QFocusEvent* event) override;
		virtual void resizeEvent(QResizeEvent* event) override;
        
    private:
		bool mIsDirty;
		
		/// \return nullptr if \p node has no corresponding widget
		WidgetNode* findWidgetFor(NodePtr node) const;
		/// Creates and returns a new WidgetNode instance (or derived
		/// type) for the given node. Does not register it with
		/// mWidgetNodes, etc.
		WidgetNode* createWidgetFor(NodePtr node);

		PipelinePtr mPipeline;
		PipelineListener* mPipelineListener;
		
		/// Owning reference
		QList<WidgetNode*> mWidgetNodes;
		/// Non-owning reference (owned by the parent node)
		QList<WidgetOutlet*> mWidgetOutlets;
		/// Non-owning reference (owned by the parent node)
		QList<WidgetInlet*> mWidgetInlets;
		/// Owning reference
		QList<WidgetPatchCord*> mWidgetPatchCords;
		
		/// If the user is midway through creating a new patch cord then this
		/// is non-null.
		WidgetNewPatchCord* mNewPatchCord;
		MouseGrabber* mMouseGrabber;
		PatchAreaMousePressFilter* mMousePressFilter;
	};
	
//	template <typename T>
//	T WidgetPatchArea::findUnderMouse() const
//	{
//		QList<T> outlets = findChildren<T>();
//		// later elements in findChild() are on top of earlier elements
//		// so search backwards
//		for (int i=outlets.size()-1; i>=0; i--)
//		{
//			if (outlets[i]->underMouse())
//			{
//				return outlets[i];
//			}
//		}
//		return nullptr;
//	}
}