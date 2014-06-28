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
		struct ParameterCreationVisitor;
		
		// Show/hide parameters based on current renderer and update
		// inlets
		void onRendererChanged();
		/// Callback for new data arriving
		void onNewData(double timestamp, int inlet);
		
		// Data
		std::atomic<double> mTimestampOfData;
		double mTimestampOfLastDraw;
		// for each inlet, we keep a cache of data for each outlet
		// from whence it arrived. This vector is filled by
		// onRendererChanged() to the correct size.
		std::vector<std::map<OutletPtr, Data>> mCache;
		/// Guards mCache AND requests by this class to remove/add inlets.
		/// Necessary because at the moment inlet callbacks
		/// are called in a different thread than the parameter update
		/// callbacks.
		boost::mutex mCacheAndInletsMutex;
		
		// Renderer
		std::vector<RendererPtr> mRenderers;
		std::vector<std::vector<ParameterPtr>> mRendererParameters;
		int mRenderer;
	};
}

#include "FactoryNode.h"
hm_register_node(NodeRenderer)

