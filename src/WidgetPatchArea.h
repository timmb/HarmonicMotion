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
		WidgetPatchArea(QWidget* parent=nullptr);
        
        /// Create a widget for an existing node
        WidgetNode* addNode(NodePtr node);
        /// Create a disconnected patch cord
        WidgetPatchCord* addPatchCord();
        /// Add an already-connected patch cord. \p inlet and \p outlet
        /// must belong to a node that has already been added to this patch
        /// area. The connection between inlet and outlet must already
        /// exist before calling this function.
        WidgetPatchCord* addPatchCord(OutletPtr outlet, InletPtr inlet);
        
        /// If there is an inlet visible at point \p position then this will
        /// return a pointer to it. Otherwise it will return nullptr.
        WidgetInlet* findInlet(QPoint position) const;
        /// If there is an inlet visible at point \p position then this will
        /// return a pointer to it. Otherwise it will return nullptr.
        WidgetOutlet* findOutlet(QPoint position) const;
        
		
	public Q_SLOTS:
		/// Request the patch area update its size. Call this if a contained
		/// moves or is resized to ensure the scroll area
		/// encapsulating this widget updates
		void updateSize();
        
	protected:
		virtual QSize sizeHint() const override;
        
    private:
        void callbackWidgetNodeDestroyed(WidgetNode*);
        
        QMap<NodePtr, WidgetNode*> mWidgetNodes;
        QMap<OutletPtr, WidgetOutlet*> mWidgetOutlets;
        QMap<InletPtr, WidgetInlet*> mWidgetInlets;
	};
}