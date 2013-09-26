//
//  Node.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 25/09/2013.
//  Copyright (c) 2013 Tim Murray-Browne. All rights reserved.
//

#pragma once

namespace hm
{
	class Node;
	typedef std::shared_ptr<Node> NodePtr;
	
	class Inlet;
	
	class Node
	{
	public:
		std::string const& type() const { return mClassName; }
		
	protected:
		Node(std::string const& className);
		
		void addInlet(std::shared_ptr<Inlet> inlet);
		void removeInlet(std::shared_ptr<Inlet> inlet);
		
	private:
		std::string mClassName;
	};
}