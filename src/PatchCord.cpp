//
//  PatchCord.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 21/02/2014.
//
//

#include "PatchCord.h"
#include "Inlet.h"
#include "Outlet.h"
#include "Pipeline.h"
#include <sstream>

namespace hm
{
	
	PatchCord::PatchCord(OutletPtr outlet, InletPtr inlet)
	: mOutlet(outlet)
	, mInlet(inlet)
	, mOutletNode(outlet->node())
	, mInletNode(inlet->node())
	, mOutletIndex(outlet->index())
	, mInletIndex(inlet->index())
	{}
	
	PatchCord::PatchCord(Json::Value const& json, PipelinePtr p)
	: mOutlet(p->outletFromPath(json["outlet"].asString()))
	, mInlet(p->inletFromPath(json["inlet"].asString()))
	, mOutletNode(mOutlet->node())
	, mInletNode(mInlet->node())
	, mOutletIndex(mOutlet->index())
	, mInletIndex(mInlet->index())
	{
	}
	
	PatchCord::~PatchCord()
	{
		
	}
	
	bool PatchCord::isValid() const
	{
		return mOutlet && mInlet;
	}
	
	bool PatchCord::operator==(PatchCord const& rhs) const
	{
		return mOutlet == rhs.mOutlet && mInlet == rhs.mInlet;
	}
	
	std::string PatchCord::toString() const
	{
		return (std::stringstream() << *this).str();
	}
	
	Json::Value& operator<<(Json::Value& j, PatchCord const& p)
	{
		j["outlet"] = p.outlet()->path();
		j["inlet"] = p.inlet()->path();
		return j;
	}
	
	std::ostream& operator<<(std::ostream& out, PatchCord const& p)
	{
		return out << "[" << (!p.isValid()? "INVALID " : "") << "Patchcord "
		<<p.outlet()->path() << " --> " << p.inlet()->path() << "]";
	}
	
}