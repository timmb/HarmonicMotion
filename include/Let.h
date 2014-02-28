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
		virtual ~Let();
		
		Types types() const { return mTypes; }
		std::string name() const { return mName; }
		std::string helpText() const { return mHelpText; }
		virtual std::string toString() const = 0;

        /// If this inlet is owned by a node and that node was was
        /// created by FactoryNode then this function will return a weak
        /// pointer to the node. Otherwise it returns
        /// std::weak_ptr<Node>(nullptr)
		/// If the owning node is midway through being destroyed then
		/// this may also return nullptr.
        std::weak_ptr<Node> node() const;

	protected:
		Let(Types types, Node& owner, std::string const& name, std::string const& helpText);
		
		
		
	private:
		/// This is used by the owning node when it is destroyed just in case
		/// the
		/// outlet outlives its node. There is no need to call it normally
		void detachOwnerNode();
	
		Node* mNode;
		Types mTypes;
		std::string mName;
		std::string mHelpText;
	};
}