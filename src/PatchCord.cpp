//
//  PatchCord.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 21/02/2014.
//
//

#include "PatchCord.h"

using namespace hm;

PatchCord::PatchCord(OutletPtr outlet, InletPtr inlet)
: mOutlet(outlet)
, mInlet(inlet)
{}

bool PatchCord::operator==(PatchCord const& rhs) const
{
	return mOutlet == rhs.mOutlet && mInlet == rhs.mInlet;
}