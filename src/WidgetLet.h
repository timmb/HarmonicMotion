//
//  WidgetLet.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 10/02/2014.
//
//

#pragma once
#include <QLabel>

namespace hm
{
    class WidgetNode;
	/// Base class for inlet and outlet widgets
	class WidgetLet : public QLabel
	{
		Q_OBJECT
        
	public:
        /// \return the coordinates of the point where connections
        /// to this Let should be drawn, relative to the containing
        /// WigetPatchArea
        QPoint connectionPoint() const;
        
    Q_SIGNALS:
        /// Emitted when the Inlet/Outlet moves.
        void positionChanged();
        
    protected:
		WidgetLet(WidgetNode* parent);
        virtual void moveEvent(QMoveEvent*) override;
        virtual void resizeEvent(QResizeEvent*) override;
        
        WidgetNode* mParent;
	};
}