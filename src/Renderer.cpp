#include "Renderer.h"
#include "cinder/gl/gl.h"
#include "Value.h"
#include "Point3d.h"
#include "Skeleton3d.h"
#include "Scene3d.h"

using namespace hm;
using namespace ci;
using namespace ci::gl;

Renderer::Renderer(std::string const& name, std::string const& description)
: mLastSceneMeta(SceneMeta::sDefaultSceneMeta)
, mName(name)
, mDescription(description)
, mNeedsRefresh(true)
{
}

void Renderer::refresh()
{
	mNeedsRefresh = true;
}


void Renderer::setupMatrices3d()
{
	mLastSceneMeta->setupCamera(mViewport.getWidth(), mViewport.getHeight());
}

void Renderer::setupMatricesWindow()
{
	gl::setMatricesWindow(mViewport.getWidth(), mViewport.getHeight());
}


void Renderer::render(Data const& data, ci::Area const& viewport)
{
	if (!(viewport == mViewport))
	{
		mViewport = viewport;
		setViewport(mViewport);
		mNeedsRefresh = true;
	}
	boost::apply_visitor(*this, data.data);
	mNeedsRefresh = false;
}


// BlobRenderer

BlobRenderer::BlobRenderer()
: Renderer("Blob", "Renders 3D points as blobs.")
{
	
}

void BlobRenderer::operator()(Point3d const& x)
{
	if (needsRefresh())
	{
		setupMatrices3d();
	}
	// TODO: Use a VBO to speed this up
	// TODO: better colour
	color(Color(0.9f, 0.4f, 0.4f));
	drawSphere(x.value, 0.05, 12);
}

void BlobRenderer::operator()(Skeleton3d const& x)
{
	for (int i=0; i<NUM_JOINTS; i++)
	{
		(*this)(x.joint(i));
	}
}

void BlobRenderer::operator()(Scene3d const& x)
{
	for (Skeleton3d const& s: x.skeletons)
	{
		(*this)(s);
	}
}









