//
//  NodeRenderer.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 23/10/2013.
//
//

#include "NodeRenderer.h"
#include "Inlet.h"
#include "cinder/gl/gl.h"

using namespace hm;
using namespace ci;

NodeRenderer::NodeRenderer(std::string const& className)
: Node(className)
, mInlet(new Inlet(ALL_TYPES, "Data to render", "Received data is stored until"
		 " a request is received to render"))
, mTimestampOfData(-42.)
, mTimestampOfLastDraw(-42.)
{
	mInlet->setNotifyCallback([this](double d){ mTimestampOfData = d; });
	addInlet(mInlet);
}

bool NodeRenderer::isRedrawRequired() const
{
	return mTimestampOfData > mTimestampOfLastDraw;
}

void NodeRenderer::draw()
{
	static ColorA clearColor = ColorA::hex(0xADFFD5);
	gl::clear(clearColor);
	Data data(mInlet->data());
	mTimestampOfLastDraw = data.timestamp();
	data.asDataType()->draw();
}