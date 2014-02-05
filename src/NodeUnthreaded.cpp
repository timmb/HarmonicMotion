#include "NodeUnthreaded.h"

namespace hm
{
	
	NodeUnthreaded::NodeUnthreaded(Node::Params params, std::string className)
	: Node(params, className)
	{}
	
	void NodeUnthreaded::startProcessing()
	{
		Node::startProcessing();
		start();
	}
	
	void NodeUnthreaded::stepProcessing()
	{
		assert(isProcessing());
		updateParameters();
		if (isEnabled())
		{
			step();
		}
	}
	
	void NodeUnthreaded::stopProcessing()
	{
		Node::stopProcessing();
		stop();
	}
	
	std::ostream& operator<<(std::ostream& out, NodeUnthreaded const& node)
	{
		return out << "Unthreaded "<< *(Node*)& node;
	}
	
}