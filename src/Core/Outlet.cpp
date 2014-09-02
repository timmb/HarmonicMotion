//
//  Outlet.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#include "Outlet.h"
#include "Inlet.h"
#include "Node.h"
#include "FactoryNode.h"
#include "PatchCord.h"
#include <algorithm>
#include <boost/thread/mutex.hpp>

using namespace std;

typedef boost::lock_guard<boost::mutex> Lock;

namespace hm
{
	
	Outlet::Outlet(Types types, Node& node, int index_, string const& name, string const& description)
	: Let(types, node, index_, name, description, "Outlet")
	{
	}
	
	
	std::ostream& operator<<(std::ostream& out, Outlet const& o)
	{
		return out << o.path() <<" ("<<o.mPatchCords.size()<<" connections)";
	}
	
	
	std::string Outlet::toString() const
	{
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}
	
	
	std::string Outlet::path() const
	{
		NodePtr n = node().lock();
		if (n==nullptr)
		{
			return "[detached]/outlets/" + to_string(index());
		}
		else
		{
			return n->path() + "/outlets/" + to_string(index());
		}
	}
	
	void Outlet::addPatchCord(PatchCordPtr patchCord)
	{
		Lock lock(mPatchCordsMutex);
		// check no equivalent patch cord has been added, and that the outlet
		// of the new one is correct
		bool isValid = !any_of(mPatchCords.begin(), mPatchCords.end(), [&](PatchCordPtr p) {
			return *p == *patchCord;
		})
		&& patchCord->outlet().get() == this;
		assert(isValid);
		if (isValid)
		{
			mPatchCords.push_back(patchCord);
		}
	}
	
	void Outlet::removePatchCord(PatchCordPtr patchCord)
	{
		Lock lock(mPatchCordsMutex);
		auto it = std::find(mPatchCords.begin(), mPatchCords.end(), patchCord);
		bool isValid = it != mPatchCords.end();
		assert(isValid);
		if (isValid)
		{
			mPatchCords.erase(it);
		}
	}
	
	vector<PatchCordPtr> Outlet::patchCords() const
	{
		Lock lock(mPatchCordsMutex);
		return mPatchCords;
	}
	
	
	void Outlet::outputNewData(Data& data)
	{
		OutletPtr self = mSelf.lock();
		if (self==nullptr)
		{
			/// If we are in the process of destruction then don't send
			/// any new data.
			return;
		}
		bool isDataTypeValid = (types() & data.type()) > 0;
		assert(isDataTypeValid);
		if (isDataTypeValid)
		{
			// Add this node to the data's history list
			deque<std::string>& history = data.asBaseData()->nodeHistory;
			{
				NodePtr n = node().lock();
				if (n != nullptr)
				{
					history.push_front(n->name());
				}
				else
				{
					history.push_front("(unknown)");
				}
			}
			auto cords = patchCords();
			for (PatchCordPtr cord: cords)
			{
//				assert(data.type() & cord->inlet()->types());
				cord->inlet()->provideNewData(data, self);
			}
		}
	}
	
	
	
	
	bool Outlet::isConnectedTo(InletPtr inlet) const
	{
		return std::any_of(mPatchCords.begin(), mPatchCords.end(), [&](PatchCordPtr p) {
			return p->inlet()==inlet;
		});

	}
	

	
	
	
	
	
}