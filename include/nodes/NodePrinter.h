//
//  OutputConsolePrintout.h
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
		struct Params : public Node::Params
		{
			Params(std::string const& name_="")
			: Node::Params(name_)
			{}
		};
		
		NodePrinter(Params const& params=Params(), std::string const& className="NodePrinter");
		
	protected:
		virtual void step() override;
		
	private:
		Params mParams;
		InletPtr mInlet;
		double mLastTimestamp;
	};
}