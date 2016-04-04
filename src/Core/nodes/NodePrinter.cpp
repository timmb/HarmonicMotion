//
//  OutputConsolePrintout.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 21/10/2013.
//
//

#include "nodes/NodePrinter.h"
#include "Inlet.h"
#include "Parameter.h"

using namespace hm;

NodePrinter::NodePrinter(Params params, std::string className)
: NodeUnthreaded(params, className, "Printer", "Print the input to the debug console.")
, mInlet(nullptr)
, mLastTimestamp(-42.)
, mMaxBufferSize(1000)
, mEnableWordWrap(false)
, mEnableFreeze(false)
{
	mInlet = createInlet(ALL_TYPES, "Input printed to console", "");
	auto paramBuffer = addParameter("Buffer size", &mMaxBufferSize, "The maximum number of received inputs that will be shown in the console's history.");
	paramBuffer->setBounds(1, 25000, 1, 1000);
	addParameter("Word wrap", &mEnableWordWrap, "Wrap lines in console output.");
	addParameter("Freeze", &mEnableFreeze, "Prevents new input from being written to the console.");
}


void NodePrinter::step()
{
	double t = mInlet->dataTimestamp();
	if (t > mLastTimestamp)
	{
		if (!mEnableFreeze)
		{
			Data data = mInlet->data();
			auto history = data.asBaseData()->nodeHistory;
			std::stringstream s;
			s << "-- " << name() << " -- " << t
				<< " " << (!history.empty() ? history[0] : "") << "\n   " << data << std::endl;
			outputConsoleText(s.str());
		}
		mLastTimestamp = t;
	}
}


NodePtr NodePrinter::create(Node::Params params) const
{
	return NodePtr(new NodePrinter(params));
}
