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
		virtual std::string toString() const = 0;
		std::string path() const;

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
		Let(Types types, Node& owner, std::string const& name, std::string const& helpText);
		
		/// This is used by the owning node when it is destroyed just in case
		/// the
		/// outlet outlives its node. There is no need to call it normally
		/// It is extended by Inlet and Outlet and not accessed directly
		/// by Node.
		virtual void detachOwnerNode();
		
	private:
		Node* mNode;
		Types mTypes;
		std::string mName;
		std::string mHelpText;
		bool mIsDetached;
	};
}