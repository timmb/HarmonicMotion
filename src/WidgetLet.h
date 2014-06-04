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
		virtual ~WidgetLet();
		
        /// \return the coordinates of the point where connections
        /// to this Let should be drawn, relative to the containing
        /// WigetPatchArea
        QPoint connectionPoint() const;
		
		/// \return the WidgetNode that owns this WidgetLet.
		WidgetNode* node() const { return mParent; }
        
    Q_SIGNALS:
        /// Emitted when the Inlet/Outlet moves.
        void positionChanged();
        
    protected:
		WidgetLet(WidgetNode* parent);
        virtual void moveEvent(QMoveEvent*) override;
        virtual void resizeEvent(QResizeEvent*) override;
		virtual void mousePressEvent(QMouseEvent*) override;
        
        WidgetNode* mParent;
	};
}