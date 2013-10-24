//
//  NodeOscOutWidget.h
//  HarmonicMotionGui
//
//  Created by Tim Murray-Browne on 24/10/2013.
//
//

#pragma once
#include <QWidget>
#include <memory>

class QLineEdit;

namespace hm
{
	class NodeOscOut;
	class NodeOscOutWidget : public QWidget
	{
		Q_OBJECT;
	public:
		typedef std::shared_ptr<NodeOscOut> NodeOscOutPtr;

		NodeOscOutWidget(NodeOscOutPtr nodeOscOut, QWidget* parent=nullptr);
		
	protected Q_SLOTS:
		void destinationHostWidgetChanged();
		void setDestinationPort(int);
		void setPrefix(QString);
		
	Q_SIGNALS:
		void newDestinationHost(QString);
		void newDestinationPort(int);
		void newPrefix(QString);
		
	private:
		std::shared_ptr<NodeOscOut> mNode;
		QLineEdit* mHostWidget;
	};
	
}