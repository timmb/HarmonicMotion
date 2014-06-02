//
//  OutputConsolePrintout.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 21/10/2013.
//
//

#include "NodePrinter.h"
#include "Inlet.h"

using namespace hm;

NodePrinter::NodePrinter(Params params, std::string className)
: NodeUnthreaded(params, className)
, mParams(params)
, mInlet(nullptr)
, mLastTimestamp(-42.)
{
	mInlet = createInlet(ALL_TYPES, "Input printed to console", "");
}


void NodePrinter::step()
{
	double t = mInlet->dataTimestamp();
	if (t > mLastTimestamp)
	{
		Data data = mInlet->data();
		auto history = data.asBaseData()->nodeHistory;
		std::cout << "-- "<<mParams.name<<" -- "<<t
		<<" "<<(!history.empty()? history[0] :"")<<"\n   "<<data<<std::endl;
		mLastTimestamp = t;
	}
}


NodePtr NodePrinter::create(Node::Params params) const
{
	return NodePtr(new NodePrinter(params));
}