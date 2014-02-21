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

namespace hm
{
    class WidgetPatchCord;
    class WidgetNode;
    class WidgetInlet;
    class WidgetOutlet;
    
    
	class WidgetPatchArea : public QWidget
	{
		Q_OBJECT
	public:
		/// \param pipeline The pipeline that this patch area will represent.
		/// Will be created if null.
		WidgetPatchArea(PipelinePtr pipeline, QWidget* parent=nullptr);
		
		PipelinePtr pipeline() const { return mPipeline; }
        
		// MARK: Functions that do not affect the underlying Pipeline model
		// TODO: Make these private?
        /// Create a widget for an existing node
        WidgetNode* addNode(NodePtr node);
        /// Add an already-connected patch cord. \p inlet and \p outlet
        /// must belong to a node that has already been added to this patch
        /// area. The connection between inlet and outlet must already
        /// exist before calling this function.
        WidgetPatchCord* addPatchCord(WidgetOutlet* outlet, WidgetInlet* inlet);
        
		// MARK: Element access and querying
		/// \return The patch cord widget between \p outlet and \inlet
        /// or nullptr if they are not connected.
        WidgetPatchCord* getPatchCord(WidgetOutlet* outlet, WidgetInlet* inlet) const;
        
        /// If there is an inlet visible at point \p position then this will
        /// return a pointer to it. Otherwise it will return nullptr.
        WidgetOutlet* findOutlet(QPoint position) const;
        /// If there is an inlet visible at point \p position then this will
        /// return a pointer to it. Otherwise it will return nullptr.
        WidgetInlet* findInlet(QPoint position) const;
        
        /// \copydoc isConnectionValid(OutletPtr, InletPtr)
		bool isConnectionValid(WidgetOutlet* outlet, WidgetInlet* inlet) const;
		
		// MARK: Functions that cause modifications the underlying Pipeline
		// model (as well as this view).
		/// Create a disconnected patch cord with the given \p outlet.
		/// \pre outlet != nullptr
        WidgetPatchCord* createPatchCord(WidgetOutlet* outlet);
		/// Create a disconnected patch cord with the given \p inlet.
		/// \pre inlet != nullptr
        WidgetPatchCord* createPatchCord(WidgetInlet* inlet);
		
		/// This will call deleteLater() on \p patchCord, so may be called
		/// by patchCord itself. The destructor of WidgetPatchCord
		/// is responsible for updating the underlying model
		void deletePatchCord(WidgetPatchCord* patchCord);
		
		/// This will call deleteLater() on \p node so may be called
		/// by node itself. The destructor of WidgetNode is responsible
		/// for updating the underlying model.
		void deleteNode(WidgetNode* node);
		
		
	public Q_SLOTS:
		/// Request the patch area update its size. Call this if a contained
		/// moves or is resized to ensure the scroll area
		/// encapsulating this widget updates
		void updateSize();
        
	protected:
		virtual QSize sizeHint() const override;
        
    private:
		PipelinePtr mPipeline;
		
		// Owning reference
		QList<WidgetNode*> mWidgetNodes;
		// Non-owning reference (owned by the parent node)
		QList<WidgetOutlet*> mWidgetOutlets;
		// Non-owning reference (owned by the parent node)
		QList<WidgetInlet*> mWidgetInlets;
		// Owning reference
		QList<WidgetPatchCord*> mWidgetPatchCords;
	};
}