//
//  Outlet.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Outlet.h"
#include "Inlet.h"

using namespace std;
using namespace hm;

Outlet::Outlet(Type type, string const& name, string const& helpText)
: mType(type)
, mName(name)
, mHelpText(helpText)
{
	
}


bool Outlet::connect(InletPtr inlet)
{
	if (inlet->type() != type())
	{
		return false;
	}
	mOutputs.push_back(inlet);
	inlet->incrementNumConnections();
	return true;
}


void Outlet::outputNewData(Data& data)
{
	hm_debug("New data at outlet: "+data.toString());
	for (InletPtr out: mOutputs)
	{
		out->provideNewData(data);
	}
}