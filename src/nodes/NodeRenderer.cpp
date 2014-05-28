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
#include "SceneMeta.h"

using namespace hm;
using namespace ci;

NodeRenderer::NodeRenderer(Params const& params, std::string const& className)
: NodeUnthreaded(params, className)
, mInlet(nullptr)
, mTimestampOfData(-42.)
, mTimestampOfLastDraw(-42.)
{
	mInlet = createInlet(ALL_TYPES, "Data to render", "Received data is stored until"
						 " a request is received to render");
	mInlet->setNotifyCallback([this](double d){ mTimestampOfData = d; });
}

bool NodeRenderer::isRedrawRequired() const
{
	return mTimestampOfData > mTimestampOfLastDraw;
}

void NodeRenderer::draw(int viewportWidth, int viewportHeight)
{
	static ColorA clearColor = ColorA::hex(0xADFFD5);
	static ColorA drawColor = ColorA::hex(0x111111);
	gl::clear(clearColor);
	gl::color(drawColor);
	Data data(mInlet->data());
	BaseData* datatype = data.asBaseData();
//	if (datatype->hasSceneMeta())
	{
		datatype->sceneMeta->setupCamera(viewportWidth, viewportHeight);
	}
	mTimestampOfLastDraw = data.timestamp();
	datatype->draw();
}