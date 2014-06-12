#include "Renderer.h"
#include "cinder/gl/gl.h"
#include "Value.h"
#include "Point3d.h"
#include "Skeleton3d.h"
#include "Scene3d.h"
#include "cinder/gl/Texture.h"

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

//void Renderer::refresh()
//{
//	mNeedsRefresh = true;
//}


void Renderer::setupMatrices3d()
{
	if (mNeedsRefresh)
	{
		mLastSceneMeta->setupCamera(mViewport.getWidth(), mViewport.getHeight());
	}
}

void Renderer::setupMatricesWindow()
{
	if (mNeedsRefresh)
	{
		gl::setMatricesWindow(mViewport.getWidth(), mViewport.getHeight());
	}
}


void Renderer::render(Data const& data, ci::Area const& viewport)
{
	if (!(viewport == mViewport))
	{
		mViewport = viewport;
		setViewport(mViewport);
	}
	mNeedsRefresh = true;
	boost::apply_visitor(*this, data.data);
}


// ------------
// BlobRenderer

RendererBlob::RendererBlob()
: Renderer("Blob", "Renders 3D points as blobs.")
{
	
}

void RendererBlob::operator()(Point3d const& x, ci::ColorA const& color)
{
	setupMatrices3d();
	// TODO: Use a VBO to speed this up
	gl::color(color);
	drawSphere(x.value, 0.05, 12);
}

void RendererBlob::operator()(Skeleton3d const& x)
{
	setupMatrices3d();
	for (int i=0; i<NUM_JOINTS; i++)
	{
		float brightness = x.jointConfidence(i) * 0.5f + 0.5f;
		bool isRight = jointSide(i)==RIGHT;
		bool isLeft = jointSide(i)==LEFT;
		ColorA color(0.2f+0.8f*int(isRight)*brightness,
					 0.5f * brightness,
					 0.2f+0.8f*int(isLeft)*brightness,
					 1.0f);
		(*this)(x.joint(i), color);
	}
}

void RendererBlob::operator()(Scene3d const& x)
{
	setupMatrices3d();
	for (Skeleton3d const& s: x.value)
	{
		(*this)(s);
	}
}

void RendererBlob::operator()(ListPoint3d const& x)
{
	setupMatrices3d();
	for (Point3d const& p: x.value)
	{
		(*this)(p);
	}
}


// Renderer2D

Renderer2D::Renderer2D()
: Renderer("2D image", "Renders 2D images")
{
	
}

void Renderer2D::operator()(Image2d const& v)
{
	setupMatricesWindow();
	
	// Letterbox the image to ensure it is not distorted by the viewport shape
	
	Vec2f offset;
	float scale(1);
	// input data aspect
	float dataAspect = float(v.value.cols)/v.value.rows;
	// viewport aspect
	float vAspect = float(mViewport.getWidth()) / mViewport.getHeight();
	if (dataAspect > vAspect)
	{
		// requires letterboxes on top and bottom
		scale = float(mViewport.getWidth()) / v.value.cols;
		offset.y = int((mViewport.getHeight() - scale * v.value.rows) / 2);
	}
	else if (dataAspect < vAspect)
	{
		// requires letterboxes on left and right
		scale = float(mViewport.getHeight()) / v.value.rows;
		offset.x = int((mViewport.getWidth() - scale * v.value.cols) / 2);
	}
	Area drawArea(offset, mViewport.getSize() - offset);
	gl::translate(offset);
	gl::scale(scale, scale);
//	gl::draw(gl::Texture(v.toSurface()), drawArea);
	gl::draw(gl::Texture(v.toSurface()));
}

void Renderer2D::operator()(Point2d const& p)
{
	setupMatricesWindow();
	// TODO: Ensure this matches any transformations created by a Image2d
	gl::color(ColorA(1.f, 1.f, 1.f, 0.9f));
	gl::drawSolidCircle(p.value, 2.5f);
}


void Renderer2D::operator()(ListPoint2d const& x)
{
	for (Point2d const& p: x.value)
	{
		(*this)(p);
	}
}






