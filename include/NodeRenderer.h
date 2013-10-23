//
//  NodeRenderer.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 23/10/2013.
//
//

#pragma once
#include "Node.h"

namespace hm
{
	/// Node that collects received data and renders it on request.
	class NodeRenderer : public Node
	{
	public:
		typedef std::shared_ptr<NodeRenderer> Ptr;
		
		NodeRenderer(std::string const& className="NodeRenderer");
		
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