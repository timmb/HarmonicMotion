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
#include "cinder/Color.h"
#include "Renderer.h"

using namespace hm;
using namespace ci;
using namespace std;

NodeRenderer::NodeRenderer(Params const& params, std::string const& className)
: NodeUnthreaded(params, className)
, mInlet(nullptr)
, mTimestampOfData(-42.)
, mTimestampOfLastDraw(-43.)
, mRenderer(0)
{
	mInlet = createInlet(ALL_TYPES, "Data to render", "Received data is stored until"
						 " a request is received to render");
	mInlet->setNotifyCallback([this](double d){ mTimestampOfData = d; });
	
	mRenderers.push_back(RendererPtr(new RendererBlob()));
	mRenderers.push_back(RendererPtr(new Renderer2D()));
	

	assert(!mRenderers.empty());
	
	
	auto param = addParameter("Renderer", &mRenderer);
	param->softMin = param->hardMin = 0;
	param->softMax = param->hardMax = mRenderers.size() - 1;
}

NodePtr NodeRenderer::create(Params params) const
{
	return NodePtr(new NodeRenderer(params));
}

bool NodeRenderer::isRedrawRequired() const
{
	return mTimestampOfData > mTimestampOfLastDraw;
}

void NodeRenderer::draw(int viewportWidth, int viewportHeight)
{
//	static ColorA clearColor = ColorA::hex(0xADFFD5);
	static ColorA clearColor = ColorA::black();
//	static ColorA drawColor = ColorA::hex(0x111111);
	gl::clear(clearColor);
//	gl::color(drawColor);
	Data data(mInlet->data());
//	BaseData* datatype = data.asBaseData();
////	if (datatype->hasSceneMeta())
//	{
//		datatype->sceneMeta->setupCamera(viewportWidth, viewportHeight);
//	}
	mTimestampOfLastDraw = data.timestamp();
	assert(0 <= mRenderer && mRenderer < mRenderers.size());
	mRenderers[mRenderer]->render(data, Area(0, 0, viewportWidth, viewportHeight));
//	datatype->draw();
}


