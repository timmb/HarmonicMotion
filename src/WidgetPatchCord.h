#pragma once

#include <QWidget>
#include <QLine>

namespace hm
{
    class WidgetInlet;
    class WidgetOutlet;
    class WidgetPatchArea;
    
    class WidgetPatchCord : public QWidget
    {
        Q_OBJECT;
    public:
        /// Create a patch cord with null inlet and outlet. Used for
        /// creating a cord before it is attached to nodes
        WidgetPatchCord(WidgetPatchArea* patchArea);
        /// Creates a patch cord for a pre-existing connection. This does
        /// not create a connection if it does not exist.
        WidgetPatchCord(WidgetPatchArea* patchArea, WidgetOutlet* outlet, WidgetInlet* inlet);
        virtual ~WidgetPatchCord();
        
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
        
    protected Q_SLOTS:
        /// Adjusts the location of this patch cord.
        void redraw();
        void outletDestroyed();
        void inletDestroyed();
        
    protected:
        void paintEvent(QPaintEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        
    private:
        /// \returns true if we are tracking the mouse waiting for a click
        /// as to where to connect the loose end of this patchcord.
        bool isMouseInteractionActive() const;
        
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
    };
}