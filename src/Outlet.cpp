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

using namespace std;

namespace hm
{
	
	Outlet::Outlet(Types types, Node& node, int index, string const& name, string const& helpText)
	: Let(types, node, index, name, helpText)
	{
	}
	
	
	std::ostream& operator<<(std::ostream& out, Outlet const& o)
	{
		out << "[Outlet " << o.name() << " ";
		for (int i=0; i<o.mPatchCords.size(); i++)
		{
			InletPtr inlet = o.mPatchCords[i]->inlet();
			NodePtr node = inlet->node().lock();
			out
			<< (node? node->name() : "(nodeless)")
			<< '/'
			<< inlet->name()
			<< (i==o.mPatchCords.size()-1? "" : ", ");
		}
		return out << ']';
	}
	
	
	std::string Outlet::toString() const
	{
		return (stringstream() << *this).str();
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
		auto it = std::find(mPatchCords.begin(), mPatchCords.end(), patchCord);
		bool isValid = it != mPatchCords.end();
		assert(isValid);
		if (isValid)
		{
			mPatchCords.erase(it);
		}
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
		bool isDataTypeValid = types() & data.type();
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
			for (PatchCordPtr cord: mPatchCords)
			{
//				assert(data.type() & cord->inlet()->types());
				cord->inlet()->provideNewData(data, self);
			}
		}
		//	for (auto outRefIt=mOutputs.begin(); outRefIt!=mOutputs.end(); )
		//	{
		//		InletPtr out = outRefIt->lock();
		//		// check the inlet still exists. if it doesn't then remove the connection
		//		if (out == nullptr)
		//		{
		//			outRefIt = mOutputs.erase(outRefIt);
		//		}
		//		else
		//		{
		//			// Only send data to inlets that support it
		//			if (out->types() & data.type())
		//			{
		//				out->provideNewData(data);
		//			}
		//			++outRefIt;
		//		}
		//	}
	}
	
	
	
	
	bool Outlet::isConnectedTo(InletPtr inlet) const
	{
		return std::any_of(mPatchCords.begin(), mPatchCords.end(), [&](PatchCordPtr p) {
			return p->inlet()==inlet;
		});
		//    for (auto weakInletPtr: mOutputs)
		//    {
		//        InletPtr i = weakInletPtr.lock();
		//        assert(i != nullptr);
		//        if (i==inlet)
		//        {
		//            return true;
		//        }
		//    }
		//    return false;
	}
	
	
	//void Outlet::disconnect(InletPtr inlet)
	//{
	//    std::weak_ptr<Inlet> weakPtr(inlet);
	//    for (auto it=mOutputs.begin(); it!=mOutputs.end(); )
	//    {
	//        InletPtr o = it->lock();
	//        assert(o != nullptr);
	//        if (o == inlet)
	//        {
	//            it = mOutputs.erase(it);
	//            return;
	//        }
	//        else
	//        {
	//            ++it;
	//        }
	//    }
	//    // If we reach here then we weren't connected to inlet.
	//    assert(false);
	//}
	
	
	
	
	
	
}