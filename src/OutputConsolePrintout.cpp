//
//  OutputConsolePrintout.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 21/10/2013.
//
//

#include "OutputConsolePrintout.h"
#include "Inlet.h"

using namespace hm;

OutputConsolePrinter::OutputConsolePrinter(std::string const& printerName, std::string const& className)
: Node(className)
, mName(printerName)
, mInlet(new Inlet(VALUE, "Input printed to console", ""))
, mLastTimestamp(-42.)
{
	addInlet(mInlet);
}


void OutputConsolePrinter::run()
{
	while (!isRequestedToStop())
	{
		if (mInlet->dataTimestamp() > mLastTimestamp)
		{
			Data data = mInlet->data();
			std::cout << "-- "<<mName<<" -- "<<mInlet->dataTimestamp()
			<<"\n   "<<data<<std::endl;
		}
		waitForNewData();
	}
}