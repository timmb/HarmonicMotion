#pragma once

#include "Node.h"

namespace hm
{
	class NodeUnthreaded : public Node
	{
	public:
		/// \see Node()
		NodeUnthreaded(Node::Params params, std::string className, std::string type, std::string description);
		
		virtual void startProcessing() override final;
		virtual bool stepProcessing() override final;
		virtual void stopProcessing() override final;
		
	protected:
		// MARK: Abstract functions to implement
		/// This function will be called by the owning pipeline when processing
		/// of data begins. It is guaranteed to be called before the first step()
		/// call and before the first step() call after a stop() call.
		/// This function is called from the main thread (Pipeline's thread).
		virtual void start() {};
		/// This function will be called by the main pipeline continually while
		/// processing is active and this node is enabled. This will be called
		/// from an arbitrary thread but not from separate threads concurrently.
		virtual void step() = 0;
		/// This function is called when processing ends and guarantees that step()
		/// will not be called again until after another call to start().
		/// This function is called from the main thread (Pipeline's thread).
		virtual void stop() {};
		
	private:
		/// Guards against stopProcessing and destruction during a
		/// step()
		mutable boost::mutex mProcessMutex;
	};
	std::ostream& operator<<(std::ostream&, NodeUnthreaded const&);
}