//
//  Outlet.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once
#include "Common.h"
#include "Data.h"

namespace hm
{
	class Outlet
	{
	public:
		Outlet(Type type, std::string const& name, std::string const& helpText);
		
		Type type() const { return mType; }
		std::string name() const { return mName; }
		std::string helpText() const { return mHelpText; }
		
		/// \return false if cannot connect with inlet due to being wrong
		/// type
		bool connect(InletPtr inlet);
		void outputNewData(Data& data);
		/// \return true if this outlet is connected to any inlets
		int numInlets() const { return mOutputs.size(); }

	private:
		
		Type mType;
		std::string mName;
		std::string mHelpText;
		std::vector<InletPtr> mOutputs;
	};
	
}