//
//  NodePrinter.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 21/10/2013.
//
//

#pragma once

#include "NodeUnthreaded.h"

namespace hm
{
	class NodePrinter : public NodeUnthreaded
	{
	public:
		NodePrinter(Params params=Params(), std::string className="NodePrinter");
		
		virtual bool hasConsoleOutput() const override { return true; }
		virtual bool isConsoleShownByDefault() const override { return true; }

	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		InletPtr mInlet;
		double mLastTimestamp;
		int mMaxBufferSize;
		bool mEnableWordWrap;
		/// Ignores new input
		bool mEnableFreeze;
	};
}


#include "FactoryNode.h"
hm_register_node(NodePrinter)

