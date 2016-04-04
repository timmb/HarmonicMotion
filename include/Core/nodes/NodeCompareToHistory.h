#pragma once

#include "NodeUnthreaded.h"
#include "nodes/NodeMovingAverage.h"

namespace hm
{
	class NodeCompareToHistory : public NodeUnthreaded
	{
	public:
		NodeCompareToHistory(Params const& params, std::string const& className = "NodeDifference");

	protected:
		virtual NodePtr create(Params params) const override;
		virtual void step() override;

	private:
		enum Comparison_
		{
			DIFF,
			MAXIMUM,
			MINIMUM,
			NUM_COMPARISONS
		};
		typedef int Comparison;
		Comparison mComparison;
		void comparisonChangedCallback();

		MovingAverage mMovingAverage;
		InletPtr mInlet;
		OutletPtr mOutlet;
		double mLastTimestamp;
	};
}
#include "FactoryNode.h"
hm_register_node(NodeCompareToHistory)