//
//  NodeRenderer.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 23/10/2013.
//
//

#pragma once
#include "NodeThreaded.h"
#include "cinder/Color.h"

namespace hm
{
	/// Node that collects received data and renders it on request.
	class NodeRenderer : public NodeThreaded
	{
	public:
		typedef std::shared_ptr<NodeRenderer> Ptr;
		struct Params : public Node::Params
		{
			Params(std::string const& name_="")
			: Node::Params(name_)
			{}
		};
		
		/// Settings applied to drawing of data to a specific outlet
		struct DrawSettings
		{
			ci::ColorA color;
		};
		
		NodeRenderer(Params const& params=Params(), std::string const& className="NodeRenderer");
		
		bool isRedrawRequired() const;
		/// This will clear the render buffer and set the matrices/camera
		void draw(int viewportWidth, int viewportHeight);
		
	protected:
		virtual void run() override {}
		
	private:
		InletPtr mInlet;
		std::atomic<double> mTimestampOfData;
		double mTimestampOfLastDraw;
	};
}