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
		OutputConsolePrinter(std::string const& printerName="printer", std::string const& className="OutputConsolePrinter");
		virtual ~OutputConsolePrinter();
		
	protected:
		virtual void run() override;
		
	private:
		std::string mName;
		InletPtr mInlet;
		double mLastTimestamp;
	};
}