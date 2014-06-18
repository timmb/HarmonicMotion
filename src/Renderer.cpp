#include "Renderer.h"
#include "cinder/gl/gl.h"
#include "Value.h"
#include "Point3d.h"
#include "Skeleton3d.h"
#include "Scene3d.h"
#include "cinder/gl/Texture.h"
#include <boost/assign.hpp>
#include "Parameter.h"

using namespace hm;
using namespace ci;
using namespace std;
using boost::assign::list_of;

Renderer::Renderer(std::string const& name_, std::string const& description_, vector<InletDescription> const& inlets_)
: mLastSceneMeta(SceneMeta::sDefaultSceneMeta)
, name(name_)
, description(description_)
, inlets(inlets_)
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


void Renderer::render(Data const& data, ci::Area const& viewport, int inlet)
{
	if (!(viewport == mViewport))
	{
		mViewport = viewport;
		gl::setViewport(mViewport);
	}
	mNeedsRefresh = true;
	boost::variant<int> inletAsVariant(inlet);
	boost::apply_visitor(*this, data.data, inletAsVariant);
}


vector<Renderer::ParameterDescription> Renderer::parameters() const
{
	return mParameterDescriptions;
}


void Renderer::setParameterDescriptions(std::vector<ParameterDescription> const&descriptions)
{
	mParameterDescriptions = descriptions;
}



// ------------
// BlobRenderer

RendererBlob::RendererBlob()
: Renderer("Blob", "Renders 3D points as blobs.",
		   list_of(InletDescription("3D points received here will be rendered as blobs.", POINT3D | SKELETON3D | SCENE3D | LIST_POINT3D)))
{
	
}

void RendererBlob::operator()(Point3d const& x, int inlet, ci::ColorA const& color)
{
	setupMatrices3d();
	// TODO: Use a VBO to speed this up
	gl::color(color);
	gl::drawSphere(x.value, 0.05, 12);
}

void RendererBlob::operator()(Skeleton3d const& x, int inlet)
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
		(*this)(x.joint(i), inlet, color);
	}
}

void RendererBlob::operator()(Scene3d const& x, int inlet)
{
	setupMatrices3d();
	for (Skeleton3d const& s: x.value)
	{
		(*this)(s, inlet);
	}
}

void RendererBlob::operator()(ListPoint3d const& x, int inlet)
{
	setupMatrices3d();
	for (Point3d const& p: x.value)
	{
		(*this)(p, inlet);
	}
}


// Renderer2D

Renderer2D::Renderer2D()
: Renderer("2D image", "Renders 2D points and images",
		   list_of
		   (InletDescription("Images or 2D points to be rendered", IMAGE2D | POINT2D | LIST_POINT2D))
		   (InletDescription("Value or a list of values determining the size of points to render.", VALUE | LIST_VALUE)))
, mValueMemoryDuration(1.)
, mValueScale(1.)
, mLastValues(vector<Value>(1, Value(1.)))
{
	vector<ParameterDescription> parameters;
//	{
//		auto p = ParameterDescription(&mValueMemoryDuration, "Time until received values are forgotten (seconds)");
//		p.hardMin = 0;
//		p.softMin = 0;
//		p.softMax = 30;
//		parameters.push_back(p);
//	}
	{
		auto p = ParameterDescription(&mValueScale, "Scale values by");
		parameters.push_back(p);
	}
	setParameterDescriptions(parameters);
}

void Renderer2D::operator()(Image2d const& v, int inlet)
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

void Renderer2D::operator()(Point2d const& p, int inlet, int count)
{
	setupMatricesWindow();
	// TODO: Ensure this matches any transformations created by a Image2d
	gl::color(ColorA(1.f, 1.f, 1.f, 0.9f));
	double size = 2.5 * mValueScale;
	if (elapsedTime() - mLastValues.timestamp < mValueMemoryDuration)
	{
		size *= mLastValues.value[std::min<int>(count, mLastValues.value.size()-1)];
	}
	gl::drawSolidCircle(p.value, size);
}


void Renderer2D::operator()(ListPoint2d const& x, int inlet)
{
	for (int i=0; i<x.value.size(); i++)
	{
		(*this)(x.value[i], inlet, i);
	}
}


void Renderer2D::operator()(Value const& x, int inlet)
{
	if (inlet==1)
	{
		mLastValues.value.assign(1, x);
	}
}


void Renderer2D::operator()(ListValue const& x, int inlet)
{
	if (inlet==1)
	{
		mLastValues = x;
	}
}






