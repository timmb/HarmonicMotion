#pragma once

#include "NodeUnthreaded.h"

namespace hm
{
	class NodeImageProcessor : public NodeUnthreaded
	{
	public:
		NodeImageProcessor(Node::Params const& params, std::string const& className="NodeImageProcessor");
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		
		virtual void step() override;
		
	private:
		enum Process_
		{
			NONE,
			BLUR,
			THRESHOLD,
			INVERT
		};
		typedef int Process;

		void processChangedCallback();
		
		InletPtr mInlet;
		OutletPtr mOutlet;
		
		Process mProcess;
		ParameterPtrT<Process> mProcessParameter;
		
		int mBlurAmount;
		
		int mThreshold;
		
		std::map<Process, std::vector<ParameterPtr>> mParametersByProcess;
		
		double mTimestamp;
	};
}


#include "FactoryNode.h"
hm_register_node(NodeImageProcessor, "Image processor", "Process 2D images (e.g. blur, threshold, etc.)")

