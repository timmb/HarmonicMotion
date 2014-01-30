//
//  OutputConsolePrintout.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 21/10/2013.
//
//

#include "OutputConsolePrinter.h"
#include "Inlet.h"

using namespace hm;

OutputConsolePrinter::OutputConsolePrinter(Params const& params, std::string const& className)
: NodeThreaded(params, className)
, mParams(params)
, mInlet(nullptr)
, mLastTimestamp(-42.)
{
	mInlet = createInlet(ALL_TYPES, "Input printed to console", "");
}


OutputConsolePrinter::~OutputConsolePrinter()
{
	stopAndWait();
}


void OutputConsolePrinter::run()
{
	while (!isRequestedToStop())
	{
		if (mInlet->dataTimestamp() > mLastTimestamp)
		{
			Data data = mInlet->data();
			auto history = data.asDataType()->nodeHistory;
			std::cout << "-- "<<mParams.name<<" -- "<<mInlet->dataTimestamp()
			<<" "<<(!history.empty()? history[0] :"")<<"\n   "<<data<<std::endl;
		}
		waitForNewData();
	}
}