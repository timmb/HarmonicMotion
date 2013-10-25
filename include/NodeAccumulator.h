#pragma once

#include "Node.h"
#include "cinder/ConcurrentCircularBuffer.h"

namespace hm
{
	/// Node that accumulates values received that may
	/// be read off using a function
	class NodeAccumulator : public Node
	{
	public:
		struct Params : public Node::Params
		{
			/// Number of messages stored. Bear in mind the
			/// memory used by the buffer may be vastly more
			/// than this as Data objects often have a lot
			/// of stuff on the heap as well.
			int bufferSize;
			
			Params(int bufferSize_=50, std::string const& name_="")
			: Node::Params(name_)
			, bufferSize(bufferSize_)
			{}
		};
		
		NodeAccumulator(Params const& params = Params(), std::string const& className="NodeAccumulator");
		virtual ~NodeAccumulator();
		
		bool isEmpty() const;
		bool isFull() const;
		/// Removes next value from buffer and returns it.
		/// If buffer is empty then fails assertion and
		/// returns null Data
		Data next();
		
	protected:
		virtual void run() override;
		
	private:
		InletPtr mInlet;
		Params mParams;
		ci::ConcurrentCircularBuffer<Data> mBuffer;
	};
}