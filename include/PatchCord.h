//
//  PatchCord.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 21/02/2014.
//
//

#pragma once

#include "Common.h"

namespace hm
{
	/// Class representing a connection between an outlet and an inlet.
	class PatchCord
	{
	public:
		OutletPtr outlet() const { return mOutlet; }
		InletPtr inlet() const { return mInlet; }
		
		bool operator==(PatchCord const& rhs) const;
		
	private:
		// Accessed only by Pipeline
		PatchCord(OutletPtr oulet, InletPtr inlet);
		
		const OutletPtr mOutlet;
		const InletPtr mInlet;
		
		friend class Pipeline;
	};
}