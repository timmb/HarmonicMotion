//
//  OutputConsolePrintout.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 21/10/2013.
//
//

#pragma once

#include "Node.h"

namespace hm
{
	class OutputConsolePrinter : public Node
	{
	public:
		struct Params : public Node::Params
		{
			Params(std::string const& name_="")
			: Node::Params(name_)
			{}
		};
		
		OutputConsolePrinter(Params const& params=Params(), std::string const& className="OutputConsolePrinter");
		virtual ~OutputConsolePrinter();
		
	protected:
		virtual void run() override;
		
	private:
		Params mParams;
		InletPtr mInlet;
		double mLastTimestamp;
	};
}