//
//  NodeRenderer.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 23/10/2013.
//
//

#include "nodes/NodeRenderer.h"
#include "Inlet.h"
#include "cinder/gl/gl.h"
#include "SceneMeta.h"
#include "cinder/Color.h"
#include "Renderer.h"
#include <boost/range/adaptor/reversed.hpp>
#include "Parameter.h"

using namespace hm;
using namespace ci;
using namespace std;

/// Visitor to create a Parameter from a Renderer::ParameterDescription
struct NodeRenderer::ParameterCreationVisitor : public boost::static_visitor<ParameterPtr>
{
	NodeRenderer& node;
	Renderer::ParameterDescription description;
	
	ParameterCreationVisitor(NodeRenderer& parent, Renderer::ParameterDescription const& description_)
	: node(parent)
	, description(description_)
	{}
	
	template <typename T>
	ParameterPtr operator()(T* value)
	{
		ParameterPtrT<T> p = node.addParameter(description.name, value);
		p->setBounds(description.hardMin, description.hardMax, description.softMin, description.softMax);
		return p;
	}
};


NodeRenderer::NodeRenderer(Params const& params, std::string const& className)
: NodeUnthreaded(params, className, "Renderer", "Visualises data.")
, mTimestampOfData(-42.)
, mTimestampOfLastDraw(-43.)
, mRenderer(0)
{
	mRenderers.push_back(RendererPtr(new Renderer3d()));
	mRenderers.push_back(RendererPtr(new Renderer2d()));
	mRenderers.push_back(RendererPtr(new RendererHistory()));
	

	assert(!mRenderers.empty());
	
	
	auto param = addParameter("Renderer", &mRenderer);
	param->addNewExternalValueCallback([this]() {
		onRendererChanged();
	});
	vector<string> rendererNames;
	for (RendererPtr r: mRenderers)
	{
		rendererNames.push_back(r->name);
	}
	param->setEnumerationLabels(rendererNames);
	
	for (RendererPtr r: mRenderers)
	{
		vector<ParameterPtr> parameters;
		vector<Renderer::ParameterDescription> descriptions = r->parameters();
		for (auto const& d: descriptions)
		{
			ParameterCreationVisitor v(*this, d);
			ParameterPtr p = boost::apply_visitor(v, d.pointer);
			parameters.push_back(p);
		}
		r->provideParameters(parameters);
		mRendererParameters.push_back(parameters);
	}
	onRendererChanged();
}

void NodeRenderer::onRendererChanged()
{
	for (int i=0; i<mRenderers.size(); i++)
	{
		for (int j=0; j<mRendererParameters[i].size(); j++)
		{
			mRendererParameters[i][j]->setVisible(mRenderer == i);
		}
	}
	boost::lock_guard<boost::mutex> lock(mCacheAndInletsMutex);
	while (numInlets() > 0)
	{
		BOOST_VERIFY(removeInlet(inlet(0)));
	}
	RendererPtr r = mRenderers[mRenderer];
	for (int i=0; i<r->inlets.size(); i++)
	{
		Renderer::InletDescription const& description = r->inlets[i];
		InletPtr inlet = createInlet(description.types, description.name);
		inlet->setNotifyCallback([this, i](double d){ onNewData(d, i); });
	}
	mCache.assign(numInlets(), map<OutletPtr, Data>());
}

void NodeRenderer::onNewData(double timestamp, int inletIndex)
{
	boost::lock_guard<boost::mutex> lock(mCacheAndInletsMutex);
	
	InletPtr i = inlet(inletIndex);
	if (i != nullptr && mCache.size() == numInlets())
	{
		mCache[inletIndex][i->dataSource()] = i->data();
		// this update of timestamp technically isn't atomic but a rare mistake
		// doesn't matter in this context.
		mTimestampOfData = max<double>(mTimestampOfData, mCache[inletIndex][i->dataSource()].timestamp());
	}
}

NodePtr NodeRenderer::create(Params params) const
{
	return NodePtr(new NodeRenderer(params));
}

bool NodeRenderer::isRedrawRequired() const
{
	return mTimestampOfData > mTimestampOfLastDraw || elapsedTime() - mTimestampOfData > 2;
}

void NodeRenderer::draw(int viewportWidth, int viewportHeight)
{
	static ColorA clearColor = ColorA::black();
	gl::clear(clearColor);
	boost::lock_guard<boost::mutex> lock(mCacheAndInletsMutex);
	assert(0 <= mRenderer && mRenderer < mRenderers.size());

	// Data older than this will be destroyed
	double const expiryTime = 3;
	double t = elapsedTime();
	
	// Construct a set of data based on mCache to pass to the renderer
	vector<pair<Data, int>> dataSet;
	for (int inletIndex=0; inletIndex<mCache.size(); inletIndex++)
	{
		map<OutletPtr, Data> & inletCache = mCache[inletIndex];
		for (auto it=inletCache.begin(); it!=inletCache.end(); )
		{
			if (t - it->second.timestamp() > expiryTime)
			{
				it = inletCache.erase(it);
			}
			else
			{
				dataSet.push_back(pair<Data, int>(it->second, inletIndex));
				++it;
			}
		}
	}
	mRenderers[mRenderer]->render(std::move(dataSet), Area(0, 0, viewportWidth, viewportHeight));
	mTimestampOfLastDraw = max<double>(mTimestampOfLastDraw, mTimestampOfData);
}





