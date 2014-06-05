#include "NodeUnthreaded.h"

namespace hm
{
	
	NodeUnthreaded::NodeUnthreaded(Node::Params params, std::string className)
	: Node(params, className)
	{}
	
	void NodeUnthreaded::startProcessing()
	{
		if (!isProcessing())
		{
		Node::startProcessing();
		start();
		assert(isProcessing());
		}
	}
	
	bool NodeUnthreaded::stepProcessing()
	{
		assert(isProcessing());
		updateParameters();
		if (isEnabled())
		{
			step();
		}
		return Node::stepProcessing();
	}
	
	void NodeUnthreaded::stopProcessing()
	{
		Node::stopProcessing();
		stop();
		assert(!isProcessing());
	}
	
	std::ostream& operator<<(std::ostream& out, NodeUnthreaded const& node)
	{
		return out << "Unthreaded "<< *(Node*)& node;
	}
	
}