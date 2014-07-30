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
#include "Let.h"
#include <boost/thread/mutex.hpp>

namespace hm
{
	class Outlet : public Let
	{
	public:
        bool isConnectedTo(InletPtr inlet) const;
		
		void outputNewData(Data& data);
		
		/// \return The number of inlets this outlet is connected to.
		int numInlets() const { return mPatchCords.size(); }
		
		virtual std::string toString() const override;
		virtual std::string path() const override;
		
		/// \return a shared pointer to this instance. This needs to be checked
		/// against nullptr in case this outlet is in the process of being destroyed.
		OutletPtr ptr() const { return mSelf.lock(); }
		
	private:
		/// Outlets are constructed with a reference to their owning node
		/// The Node will automatically have this outlet registered with
		/// it.
		/// Outlets are only created by Nodes
		Outlet(Types type, Node& owner, int index, std::string const& name, std::string const& description);
		
		
		/// Connections are all managed exclusively by the owning pipeline.
		/// Responsibility lies with the pipeline to ensure any added
		/// patch cords are valid (i.e. they do not exist and there is
		/// at least one shared datatype available between the outlet
		/// and inlet).
		/// Responsibility lies with Pipeline to ensure \p p is valid and
		/// that \p p is removed when this node is removed from the pipeline
		/// (failure to do so will cause a memory leak due to the symmetric
		/// shared_ptr references).
		void addPatchCord(PatchCordPtr patchCord);
		
		/// Connections are all managed exclusively by the owning pipeline.
		/// Responsibility lies with the pipeline to ensure patchCords are
		/// removed from both outlet and inlet when removed from either.
		void removePatchCord(PatchCordPtr patchCord);
		
		std::vector<PatchCordPtr> patchCords() const;

		std::vector<PatchCordPtr> mPatchCords;
		mutable boost::mutex mPatchCordsMutex;
		
		/// Outlet maintains a weak pointer to itself. This is set by Node when
		///	Outlet is created.
		std::weak_ptr<Outlet> mSelf;
		
		friend class Node;
		friend class Pipeline;
		friend std::ostream& operator<<(std::ostream&, Outlet const&);
	};
	
	std::ostream& operator<<(std::ostream&, Outlet const&);
}


