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
using boost::assign::map_list_of;

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
		mLastSceneMeta->setupWindowLetterbox(mViewport.getWidth(), mViewport.getHeight());
	}
}


void Renderer::render(std::vector<std::pair<Data, int>> dataSet, ci::Area const& viewport)
{
	if (!(viewport == mViewport))
	{
		mViewport = viewport;
		gl::setViewport(mViewport);
	}
	mNeedsRefresh = true;
	render(std::move(dataSet));
}

void Renderer::render(std::vector<std::pair<Data, int>> dataSet)
{
	// The default order in which different types are passed to the rener
	// functions
	static int tmp = 0;
	static map<Type, int> typeOrder = map_list_of
	(IMAGE2D, tmp++)
	(SCENE3D, tmp++)
	(SKELETON3D, tmp++)
	(LIST_POINT3D, tmp++)
	(POINT3D, tmp++)
	(LIST_POINT2D, tmp++)
	(POINT2D, tmp++)
	(LIST_VALUE, tmp++)
	(VALUE, tmp++)
	(UNDEFINED, tmp++);
	static_assert(IMAGE2D|SCENE3D|SKELETON3D|LIST_POINT3D|POINT3D|LIST_POINT2D|POINT2D|LIST_VALUE|VALUE|UNDEFINED == ALL_TYPES, "Renderer type order is not defined for all types");
	// Comparer function to sort dataSet
	static auto comparer = [](pair<Data, int> x, pair<Data, int> y) {
		// x happens first if inlet is higher, or inlets are same and type is
		// earlier in the typeOrder list
		return x.second > y.second || (x.second==y.second && typeOrder[x.first.type()] < typeOrder[y.first.type()]);
	};
	
	std::sort(begin(dataSet), end(dataSet), comparer);
	for (auto dataPair: dataSet)
	{
		boost::variant<int> inletContainer(dataPair.second);
		boost::apply_visitor(*this, dataPair.first.data, inletContainer);
	}
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
	gl::draw(gl::Texture(v.toSurface()));
}

void Renderer2D::operator()(Point2d const& p, int inlet, int count)
{
	if (inlet==0)
	{
		setupMatricesWindow();
		gl::color(ColorA(1.f, 1.f, 1.f, 0.9f));
		double size = 2.5 * mValueScale;
		hm_debug(elapsedTime() - mLastValues.timestamp);
		if (!mLastValues.value.empty() && elapsedTime() - mLastValues.timestamp < mValueMemoryDuration)
		{
			hm_debug("orig size "<<size<<" value "<<mLastValues.value[std::min<int>(count, mLastValues.value.size()-1)])
			size *= mLastValues.value[std::min<int>(count, mLastValues.value.size()-1)];
		}
		gl::drawSolidCircle(p.value, size);
	}
}


void Renderer2D::operator()(ListPoint2d const& x, int inlet)
{
	if (inlet==0)
	{
		for (int i=0; i<x.value.size(); i++)
		{
			(*this)(x.value[i], inlet, i);
		}
	}
}


void Renderer2D::operator()(Value const& x, int inlet)
{
	if (inlet==1)
	{
		mLastValues.value.assign(1, x);
		mLastValues.timestamp = x.timestamp;
	}
}


void Renderer2D::operator()(ListValue const& x, int inlet)
{
	if (inlet==1)
	{
		mLastValues = x;
	}
}






