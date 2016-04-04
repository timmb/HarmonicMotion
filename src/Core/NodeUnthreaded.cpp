#include "NodeUnthreaded.h"

namespace hm
{
	
	NodeUnthreaded::NodeUnthreaded(Node::Params params, std::string className, std::string type, std::string description)
	: Node(params, className, type, description)
	{}
	
	void NodeUnthreaded::startProcessing()
	{
		boost::lock_guard<boost::mutex> lock(mProcessMutex);
		if (!isProcessing())
		{
			Node::startProcessing();
			start();
			assert(isProcessing());
		}
	}
	
	bool NodeUnthreaded::stepProcessing()
	{
		boost::lock_guard<boost::mutex> lock(mProcessMutex);
		// It's possible a thread might still call this function after stop processing has been called by the main thread.
		if (!isProcessing())
		{
			return false;
		}
		updateParameters();
		if (isEnabled())
		{
			step();
		}
		return Node::stepProcessing();
	}
	
	void NodeUnthreaded::stopProcessing()
	{
		boost::lock_guard<boost::mutex> lock(mProcessMutex);
		Node::stopProcessing();
		stop();
		assert(!isProcessing());
	}
	
	std::ostream& operator<<(std::ostream& out, NodeUnthreaded const& node)
	{
		return out << "Unthreaded "<< *(Node*)& node;
	}
	
}