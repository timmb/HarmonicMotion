//
//  Inlet.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once
#include "Common.h"

namespace hm
{
	class Inlet
	{
	public:
		Inlet(Type type, std::string const& name, std::string const& helpText);
		
		Type type() const { return mType; }
		virtual std::string name() const { return mName; }
		virtual std::string helpText() const { return mHelpText; }
		
	private:
		Type mType;
		std::string mName;
		std::string mHelpText;
	};
}