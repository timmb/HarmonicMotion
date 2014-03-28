//
//  PatchCord.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 21/02/2014.
//
//

#pragma once

#include "Common.h"
#include "json/json.h"

namespace hm
{
	/// Class representing a connection between an outlet and an inlet.
	class PatchCord
	{
	public:
		OutletPtr outlet() const { return mOutlet; }
		InletPtr inlet() const { return mInlet; }
		
		bool operator==(PatchCord const& rhs) const;
		
		/// \return true if both outlet and inlet are valid.
		bool isValid() const;
		std::string toString() const;
		
	private:
		// Accessed only by Pipeline
		PatchCord(OutletPtr oulet, InletPtr inlet);
		/// Construct a patch chord based on a JSON element.
		PatchCord(Json::Value const& json, PipelinePtr p);
		
		const OutletPtr mOutlet;
		const InletPtr mInlet;
		
		friend class Pipeline;
	};
	
	Json::Value& operator<<(Json::Value&, PatchCord const&);
	std::ostream& operator<<(std::ostream&, PatchCord const&);
}