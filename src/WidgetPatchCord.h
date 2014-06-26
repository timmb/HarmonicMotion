#pragma once

#include <QWidget>
#include <QLine>
#include "Common.h"

namespace hm
{
    class WidgetInlet;
    class WidgetOutlet;
    class WidgetPatchArea;
	class WidgetLet;
    
    class WidgetPatchCord : public QWidget
    {
        Q_OBJECT;
    public:
        /// Creates a patch cord for a pre-existing connection. This does
        /// not create a connection if it does not exist.
		/// \note WidgetPatchCord should not be constructed directly - only
		/// through the functions in WidgetPatchArea
        WidgetPatchCord(WidgetPatchArea* patchArea, WidgetOutlet* outlet, WidgetInlet* inlet);
        virtual ~WidgetPatchCord();
		
//		/// Request that this patch cord is deleted without updating the
//		/// underlying model. This is used if the model has been changed
//		/// externally and we need to update our widget representation of
//		/// it
//		void eraseWithoutUpdatingModel();
		
		/// \return the inlet/outlet of this patch cord, or nullptr if it
		/// does not yet have an inlet/outlet.
		WidgetOutlet* outlet() { return mOutlet; }
		WidgetOutlet const* outlet() const { return mOutlet; }
		/// \copydoc outlet()
		WidgetInlet* inlet() { return mInlet; }
		WidgetInlet const* inlet() const { return mInlet; }
		
		/// Return the QLine object that describes the line drawn by this widget.
		QLine line() const { return mLine; }
		
		/// \return true if \p point is near to the line that is drawn
		/// to represent this patch cord.
		/// \param point is pixel coordinates of a point relative to the
		/// origin of this widget.
		bool isLineNear(QPoint const& point) const;
		
		/// To avoid issues with mouse event propagation, we handle click
		/// events manually on WidgetPatchCord
		void myMousePressEvent(QMouseEvent const& event);
		        
    protected Q_SLOTS:
        /// Adjusts the location of this patch cord.
        void redraw();
//		void loadStyleSheet();
		/// Request that the pipeline delete this patch cord.
		void deleteFromModel();
        
    protected:
        virtual void paintEvent(QPaintEvent* event) override;
		virtual void focusInEvent(QFocusEvent* event) override;
		virtual void focusOutEvent(QFocusEvent* event) override;
        
    private:
        
        /// connect mOutlet's signals to this
        void connectOutletSignals();
        /// Connect mInlet's signals to this
        void connectInletSignals();
        void disconnectOutletSignals();
        void disconnectInletSignals();
        
        WidgetOutlet* mOutlet;
        WidgetInlet* mInlet;
        
        /// What we draw
        QLine mLine;
        
        WidgetPatchArea* mPatchArea;
		
//		bool mHasBeenErased;
		/// Remember the position of the mouse when clicked to test if we should
		/// receive focus. This position is relative to this widget.
		QPointF mMouseClickPosition;
		
		// for debugging
		std::string mDescription;
    };
	
}