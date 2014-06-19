//
//  Let.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 28/02/2014.
//
//

#pragma once
#include "Common.h"

namespace hm
{
	/// Common base class for inlets and outlets
	class Let
	{
	public:
		virtual ~Let() {}
		
		Types types() const { return mTypes; }
		std::string name() const { return mName; }
		std::string helpText() const { return mHelpText; }
		/// The index of a Let identifies which inlet (or outlet) this
		/// is for a given Node, starting at 0, 1, etc. An inlet may
		/// share an index with an outlet for a given node.
		int index() const { return mIndex; }
		virtual std::string toString() const = 0;
		/// The path of a Let is, e.g., /<node-name>/inlets/0 where 0 is
		/// the index of the node
		virtual std::string path() const = 0;

        /// If this inlet is owned by a node and that node was was
        /// created by FactoryNode then this function will return a weak
        /// pointer to the node. Otherwise it returns
        /// std::weak_ptr<Node>(nullptr)
		/// If the owning node is midway through being destroyed or has
		/// detached this Let then
		/// this may also return nullptr.
		/// When locking, always check that this is not nullptr
        std::weak_ptr<Node> node() const;

		
		/// \return Whether this Let has been detached from its owning
		/// Node. A detached Let is waiting to be destroyed and will
		/// not perform any normal actions
		/// isDetached() => node() == nullptr
		bool isDetached() const { return mIsDetached; }
		
	protected:
		Let(Types types, Node& owner, int index, std::string const& name, std::string const& helpText);
		
		/// This is used by the owning node when it is destroyed just in case
		/// the
		/// outlet outlives its node. There is no need to call it normally
		/// It is extended by Inlet and Outlet and not accessed directly
		/// by Node.
		virtual void detachOwnerNode();
		
	private:
		Types mTypes;
		Node* mNode;
		int mIndex;
		std::string mName;
		std::string mHelpText;
		bool mIsDetached;
	};
}