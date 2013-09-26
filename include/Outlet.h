//
//  Outlet.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once
#include "Common.h"

namespace hm
{
	class Outlet
	{
	public:
		Outlet(Type type, std::string const& name, std::string const& helpText);
		
		Type type() const { return mType; }
		std::string name() const { return mName; }
		std::string helpText() const { return mHelpText; }
		bool hasConnections() const;
		
		void incrementNumConnections();
		void decrementNumConnections();
		
		bool waitForNewData(int maxMillisToWait);
		Data data();
		
		void outputNewData(Data& data);

	private:
		Type mType;
		std::string mName;
		std::string mHelpText;
		int mNumConnections;
		Data mData;
	};
	
}