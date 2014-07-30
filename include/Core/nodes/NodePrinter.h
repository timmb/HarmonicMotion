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
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		Params mParams;
		InletPtr mInlet;
		double mLastTimestamp;
	};
}


#include "FactoryNode.h"
hm_register_node(NodePrinter)

