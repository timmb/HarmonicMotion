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

namespace hm
{
    class WidgetPatchCord;
    class WidgetNode;
    class WidgetInlet;
    class WidgetOutlet;
	class WidgetLet;
    
    
	/// WidgetPatchArea manages a Pipeline.
	class WidgetPatchArea : public QWidget, public Pipeline::Listener
	{
		Q_OBJECT
	public:
		/// \param pipeline The pipeline that this patch area will represent.
		/// Will be created if null.
		WidgetPatchArea(PipelinePtr pipeline, QWidget* parent=nullptr);
		
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
		/// If there is an inlet or outlet visible at point \p position then
		/// this will return a pointer to it. Otherwise it will return
		/// nullptr.
		WidgetLet* findLet(QPoint position) const;
        
        /// \copydoc isConnectionValid(OutletPtr, InletPtr)
		bool isConnectionValid(WidgetOutlet* outlet, WidgetInlet* inlet) const;
		
		// MARK: Functions that cause modifications the underlying Pipeline
		// model (as well as this view).
		
		/// This will call deleteLater() on \p patchCord, so may be called
		/// by patchCord itself. The destructor of WidgetPatchCord
		/// is responsible for updating the underlying model
		void erasePatchCord(WidgetPatchCord* patchCord);
		
		/// This will call deleteLater() on \p node so may be called
		/// by node itself. The destructor of WidgetNode is responsible
		/// for updating the underlying model.
		void eraseNode(WidgetNode* node);
		
		// MARK: Other functions
		/// This is called by WidgetLet when it receives a mouse press
		/// event.
		/// \param position is the coordinates of the mouse relative to
		/// this WidgetPatchArea.
		void mousePressEventFromWidgetLet(WidgetLet* let, QPoint position);
		
		
		// MARK: Pipeline::Listener functions
		// These allow the widget to stay updated when the pipeline changes
		virtual void nodeAdded(NodePtr node);
		virtual void nodeRemoved(NodePtr node);
		virtual void patchCordAdded(OutletPtr outlet, InletPtr inlet);
		virtual void patchCordRemoved(OutletPtr outlet, InletPtr inlet);

		
	public Q_SLOTS:
		/// Request the patch area update its size. Call this if a contained
		/// moves or is resized to ensure the scroll area
		/// encapsulating this widget updates
		void updateSize();
		/// Provide new Info Panel data to this patch area, to be forwarded
		/// on to the info panel
		void provideInfoPanelText(QString);
        
	Q_SIGNALS:
		/// New text to be sent to the info panel
		void newInfoPanelText(QString);
		
	protected:
		virtual QSize sizeHint() const override;
//		virtual void mousePressEvent(QMouseEvent*) override;
		virtual void mouseMoveEvent(QMouseEvent*) override;
		virtual void focusInEvent(QFocusEvent* event) override;
        
    private:
		// MARK: Functions that do not affect the underlying Pipeline model
        /// Create a widget for an existing node
        WidgetNode* addNode(NodePtr node);
        /// Add an already-connected patch cord. \p inlet and \p outlet
        /// must belong to a node that has already been added to this patch
        /// area. The connection between inlet and outlet must already
        /// exist before calling this function.
        WidgetPatchCord* addPatchCord(WidgetOutlet* outlet, WidgetInlet* inlet);
        
		/// Create a disconnected patch cord with the given \p outlet.
		/// \pre outlet != nullptr
        WidgetPatchCord* createPatchCord(WidgetOutlet* outlet);
		/// Create a disconnected patch cord with the given \p inlet.
		/// \pre inlet != nullptr
        WidgetPatchCord* createPatchCord(WidgetInlet* inlet);
		
//		/// Find topmost child widget of type *T under the mouse. Returns
//		/// nullptr
//		/// if no widget of type *T is under mouse.
//		/// T should be a pointer type, e.g. WidgetInlet*.
//		template <typename T>
//		T findUnderMouse() const;

		PipelinePtr mPipeline;
		
		/// Owning reference
		QList<WidgetNode*> mWidgetNodes;
		/// Non-owning reference (owned by the parent node)
		QList<WidgetOutlet*> mWidgetOutlets;
		/// Non-owning reference (owned by the parent node)
		QList<WidgetInlet*> mWidgetInlets;
		/// Owning reference
		QList<WidgetPatchCord*> mWidgetPatchCords;
		
		/// If the user is currently creating a patch cord it is kept here.
		/// It is not added to mWidgetPatchCords until it is removed from
		/// this pointer. If we're not creating a patch cord then this
		/// will equal nullptr.
		WidgetPatchCord* mNewPatchCord;
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