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
			MEDIAN_BLUR,
			THRESHOLD,
			INVERT,
			RESIZE,
			//DENOISE,
			CLAMP,
			NUM_PROCESSES
		};
		typedef int Process;

		void processChangedCallback();
		
		InletPtr mInlet;
		OutletPtr mOutlet;
		
		Process mProcess;
		ParameterPtrT<Process> mProcessParameter;
		
		// Blur
		int mBlurAmount;
		
		// Threshold
		int mThreshold;

		// Resize
		int mWidth;
		int mHeight;
		bool mLockAspectRatio;
		float mPrevAspectRatio;
		int mInterpolationMethod;

		//// Denoise
		//float mDenoiseAmount;
		//int mTemplateWindowDiameter;
		//int mSearchWindowDiameter;

		// Clamp
		bool mEnableClampMin;
		float mClampMin;
		ParameterPtr mClampMinParameter;
		bool mEnableClampMax;
		float mClampMax;
		ParameterPtr mClampMaxParameter;

		// Median blur
		int mMedianBlurAmount;

		std::map<Process, std::vector<ParameterPtr>> mParametersByProcess;
		
		double mTimestamp;
		bool mHeightWasAdjusted, mWidthWasAdjusted;
	};
}


#include "FactoryNode.h"
hm_register_node(NodeImageProcessor)

