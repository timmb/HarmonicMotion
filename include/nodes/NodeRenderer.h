//
//  NodeRenderer.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 23/10/2013.
//
//

#pragma once
#include "NodeUnthreaded.h"
#include "Common.h"

namespace hm
{
	/// Node that collects received data and renders it on request.
	class NodeRenderer : public NodeUnthreaded
	{
	public:		
		NodeRenderer(Params const& params=Params(), std::string const& className="NodeRenderer");
		
		bool isRedrawRequired() const;
		/// This will clear the render buffer and render the most recent
		/// data to have arrived
		void draw(int viewportWidth, int viewportHeight);
		
	protected:
		virtual void step() override {}
		virtual NodePtr create(Params params) const override;
		
	private:
		InletPtr mInlet;
		std::atomic<double> mTimestampOfData;
		double mTimestampOfLastDraw;
		
		std::vector<RendererPtr> mRenderers;
		int mRenderer;
	};
}

#include "FactoryNode.h"
hm_register_node(NodeRenderer, "Renderer", "Renders data received to an Open GL context.")

