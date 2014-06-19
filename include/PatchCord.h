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
		virtual ~PatchCord();
		
		OutletPtr outlet() const { return mOutlet; }
		InletPtr inlet() const { return mInlet; }
		
		bool operator==(PatchCord const& rhs) const;
		
		/// \return true if both outlet and inlet are valid.
		bool isValid() const;
		std::string toString() const;
		
		/// Patchcord retains a reference to the index and node of
		/// the outlet/inlet, which will remain valid after the lets
		/// have been detached from the node. However, the node pointer
		/// needs to be checked that it is not nullptr.
		std::weak_ptr<Node> outletNode() const { return mOutletNode; }
		std::weak_ptr<Node> inletNode() const { return mInletNode; }
		int outletIndex() const { return mOutletIndex; }
		int inletIndex() const { return mInletIndex; }
		
	private:
		// Accessed only by Pipeline
		PatchCord(OutletPtr oulet, InletPtr inlet);
		/// Construct a patch chord based on a JSON element.
		PatchCord(Json::Value const& json, PipelinePtr p);
		
		OutletPtr const mOutlet;
		InletPtr const mInlet;
		
		// NB these members must appear after mOutlet and mInlet in this
		// class definition.
		std::weak_ptr<Node> const mOutletNode;
		std::weak_ptr<Node> const mInletNode;
		int const mOutletIndex;
		int const mInletIndex;
		
		friend class Pipeline;
	};
	
	Json::Value& operator<<(Json::Value&, PatchCord const&);
	std::ostream& operator<<(std::ostream&, PatchCord const&);
}