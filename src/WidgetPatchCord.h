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
        /// Create a patch cord with null inlet and outlet. Used for
        /// creating a cord before it is attached to nodes.
		/// \note WidgetPatchCord should not be constructed directly - only
		/// through the functions in WidgetPatchArea
        WidgetPatchCord(WidgetPatchArea* patchArea);
        /// Creates a patch cord for a pre-existing connection. This does
        /// not create a connection if it does not exist.
		/// \note WidgetPatchCord should not be constructed directly - only
		/// through the functions in WidgetPatchArea
        WidgetPatchCord(WidgetPatchArea* patchArea, WidgetOutlet* outlet, WidgetInlet* inlet);
        virtual ~WidgetPatchCord();
		
		/// Request that this patch cord is deleted. Updates the underlying
		/// model. This widget instance will be deleted during the next
		/// cycle of the event loop.
		void erase();
		
		/// Request that this patch cord is deleted without updating the
		/// underlying model. This is used if the model has been changed
		/// externally and we need to update our widget representation of
		/// it
		void eraseWithoutUpdatingModel();
        
        /// \copydoc setInlet
        void setOutlet(WidgetOutlet* outlet);
        /// Changes the inlet/outlet for this cord to the provided value,
        /// updating
        /// the relevant Nodes, inlets and outlets corresponding to these
        /// widgets.
        /// If only one of inlet or outlet is set then this patchcord will
        /// be considered as being actively edited and the other end will
        /// track the mouse
        void setInlet(WidgetInlet* inlet);
		
		/// \return the inlet/outlet of this patch cord, or nullptr if it
		/// does not yet have an inlet/outlet.
		WidgetOutlet* outlet() { return mOutlet; }
		/// \copydoc outlet()
		WidgetInlet* inlet() { return mInlet; }
		
		// MARK: Functions for interactive mouse-based creation of patchcords
        /// \return whether this patchcord has either an inlet or an outlet
		/// connected but not both.
        bool isPartiallyConnected() const;
		/// \return true if both inlet and outlet have been set
		bool isFullyConnected() const;
		/// If this patch cord is only connected at one end then the location
		/// that the other end is drawn can be set with this. This function
		/// is used by WidgetPatchArea while a patch is being drawn by the
		/// user.
		/// \pre This patch cord must have either an inlet or an outlet but
		/// not both
		void setUnconnectedPointDrawPosition(QPoint position);
		/// Checks to see if WidgetLet could be set to the unconnected part
		/// of this patch cord. If WidgetLet is an outlet but we need an
		/// inlet then this returns false, and vice versa. Also if the
		/// datatype do not match then this returns false.
		/// If the patch cord is not partially connected
		/// this fails an assertion and returns false.
		bool trySettingUnconnectedLet(WidgetLet* let);
        
    protected Q_SLOTS:
        /// Adjusts the location of this patch cord.
        void redraw();
//        void outletDestroyed();
//        void inletDestroyed();
//		void loadStyleSheet();
        
    protected:
        virtual void paintEvent(QPaintEvent* event) override;
//        virtual void mousePressEvent(QMouseEvent* event) override;
//        virtual void mouseMoveEvent(QMouseEvent* event) override;
        
    private:
        
        /// connect mOutlet's signals to this
        void connectOutletSignals();
        /// Connect mInlet's signals to this
        void connectInletSignals();
        void disconnectOutletSignals();
        void disconnectInletSignals();
        
        /// Connects the InletPtr and OutletPtr on the underlying model.
        /// \pre mInlet != nullptr && mOutlet!-nullptr && underlying outlet
        /// is connected to underlying inlet
        void connectUnderlyingModel();
        /// Disconnects the InletPtr and OutletPtr on the underlying model
        /// \pre mInlet != nullptr && mOutlet != nnullptr && underlying
        /// outlet is not connected to underlying inlet
        void disconnectUnderlyingModel();
        
        WidgetOutlet* mOutlet;
        WidgetInlet* mInlet;
        
        /// What we draw
        QLine mLine;
        bool mIsLineDrawn;
        
        WidgetPatchArea* mPatchArea;
		
		bool mHasBeenErased;
    };
}