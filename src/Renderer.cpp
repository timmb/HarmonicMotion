#include "Renderer.h"
#include "cinder/gl/gl.h"
#include "Value.h"
#include "Point3d.h"
#include "Skeleton3d.h"
#include "Scene3d.h"
#include "cinder/gl/Texture.h"
#include <boost/assign.hpp>
#include "Parameter.h"
#include <boost/range/adaptor/reversed.hpp>

using namespace hm;
using namespace ci;
using namespace std;
using boost::assign::list_of;
using boost::assign::map_list_of;
using boost::adaptors::reversed;

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
// Renderer3d

Renderer3d::Renderer3d()
: Renderer("Blob", "Renderer for 3D data such as Skeletons and Scenes.",
		   list_of
		   (InletDescription("3D points received here will be rendered as blobs.", POINT3D | SKELETON3D | SCENE3D | LIST_POINT3D))
		   (InletDescription("3D or Value data received here will modify how points delivered to the first inlet are rendered. For example a Scene3d here will cause lines to stick out of the joints of a Scene3d provided in the first inlet.", POINT3D | SKELETON3D | SCENE3D | LIST_POINT3D | VALUE | LIST_VALUE))
		   )
{
	
}

void Renderer3d::operator()(Point3d const& x, int inlet)
{
	if (inlet==0)
	{
		cleanDescriptors(x.timestamp);
		gl::color(ci::ColorA::white());
		// Search for a descriptor for this value
		cleanDescriptors(x.timestamp);
		Data const& descriptor = getDescriptor(x.id, POINT3D | VALUE);
		if (descriptor.isPoint3d())
		{
			drawPoint(x, descriptor.asPoint3d());
		}
		else if (descriptor.isValue())
		{
			assert(descriptor.isValue());
			drawPoint(x, descriptor.asValue());
		}
		else
		{
			drawPoint(x, 1.);
		}
	}
	else
	{
		assert(inlet == 1);
		addDescriptor(x);
	}
}

void Renderer3d::drawPoint(Point3d const& x, Point3d const& descriptor)
{
	setupMatrices3d();
	// TODO: Use a VBO to speed this up
	gl::drawSphere(x.value, 0.05, 12);
	gl::drawLine(x.value, x.value+descriptor.value * 4);
}

void Renderer3d::drawPoint(Point3d const& x, Value const& descriptor)
{
	gl::drawSphere(x.value, 0.05*descriptor.value);
}

void Renderer3d::operator()(Skeleton3d const& x, int inlet)
{
	if (inlet==0)
	{
		cleanDescriptors(x.timestamp);
		Data const& descriptor = getDescriptor(x.id, SKELETON3D | LIST_VALUE);
		if (descriptor.isSkeleton3d())
		{
			drawSkeleton(x, descriptor.asSkeleton3d());
		}
		else if (descriptor.isListValue())
		{
			drawSkeleton(x, descriptor.asListValue());
		}
		else
		{
			drawSkeletonLines(x);
		}
	}
	else
	{
		assert(inlet==1);
		addDescriptor(x);
	}
}

void Renderer3d::drawSkeleton(Skeleton3d const& x, Skeleton3d const& descriptor)
{
	drawSkeletonLines(x);
	gl::color(ColorA::white());
	for (int i=0; i<NUM_JOINTS; i++)
	{
		drawPoint(x.joint(i), descriptor.joint(i));
	}
}

void Renderer3d::drawSkeleton(Skeleton3d const& x, ListValue const& descriptor)
{
	drawSkeletonLines(x);
	gl::color(ColorA::white());
	for (int i=-0; i<NUM_JOINTS; i++)
	{
		// see if we have a descriptor for this joint
		auto it = find_if(descriptor.value.begin(), descriptor.value.end(), [i](Value const& v) {
			return v.id == i;
		});
		if (it != descriptor.value.end())
		{
			drawPoint(x.joint(i), *it);
		}
	}
}
	
void Renderer3d::drawSkeletonLines(Skeleton3d const& x)
{
	setupMatrices3d();
	for (int i=0; i<NUM_LIMBS; i++)
	{
		float startBrightness = x.jointConfidence(limbJoints(i).first) * 0.5f + 0.5f;
		float endBrightness = x.jointConfidence(limbJoints(i).second) * 0.5f + 0.5f;
		bool isRight = limbSide(i)==RIGHT;
		bool isLeft = limbSide(i)==LEFT;
		ColorA startColor(0.2f+0.8f*int(isRight)*startBrightness,
					 0.5f * startBrightness,
					 0.2f+0.8f*int(isLeft)*startBrightness,
					 1.0f);
		ColorA endColor(0.2f+0.8f*int(isRight)*endBrightness,
						  0.5f * endBrightness,
						  0.2f+0.8f*int(isLeft)*endBrightness,
						  1.0f);
//		gl::color(ColorA::white());
		glBegin(GL_LINES);
		gl::color(startColor);
		gl::vertex(x.joint(limbJoints(i).first).value);
		gl::color(endColor);
		gl::vertex(x.joint(limbJoints(i).second).value);
		glEnd();
//		gl::drawLine(x.joint(limbJoints(i).first).value, x.joint(limbJoints(i).second).value);
		
	}
}


void Renderer3d::operator()(Scene3d const& x, int inlet)
{
	if (inlet==0)
	{
		cleanDescriptors(x.timestamp);
		setupMatrices3d();
		Data descriptor_ = getDescriptor(x.id, SCENE3D);
		static Scene3d emptyScene3d;
		// if we don't find a descriptor then use an empty Scene3d
		Scene3d const& descriptor = descriptor_.isScene3d()? descriptor_.asScene3d() : emptyScene3d;
		for (int i=0; i<x.value.size(); i++)
		{
			auto it = find_if(descriptor.value.begin(), descriptor.value.end(), [i](Skeleton3d const& s){
				return s.id == i;
			});
			if (it!=descriptor.value.end())
			{
				drawSkeleton(x.value[i], *it);
			}
			else
			{
				drawSkeletonLines(x.value[i]);
			}
		}
	}
	else
	{
		assert(inlet=1);
		addDescriptor(x);
	}
}

void Renderer3d::operator()(ListPoint3d const& x, int inlet)
{
	if (inlet==0)
	{
		cleanDescriptors(x.timestamp);
		Data descriptor_ = getDescriptor(x.id, LIST_POINT3D | LIST_VALUE);
		if (descriptor_.isListPoint3d())
		{
			ListPoint3d const& descriptor = descriptor_.asListPoint3d();
			for (int i=0; i<x.value.size(); i++)
			{
				auto it = find_if(descriptor.value.begin(), descriptor.value.end(), [i](Point3d const& p) {
					return p.id == i;
				});
				if (it!=descriptor.value.end())
				{
					drawPoint(x.value[i], *it);
				}
				else
				{
					drawPoint(x.value[i], 1.);
				}
			}
		}
		else
		{
			assert(descriptor_.type() & (UNDEFINED | LIST_VALUE));
			// if descriptor_ is null then use an empty value list
			static ListValue emptyListValue;
			ListValue const& descriptor = descriptor_.isListValue()? descriptor_.asListValue() : emptyListValue;
			for (int i=0; i<x.value.size(); i++)
			{
				auto it = find_if(descriptor.value.begin(), descriptor.value.end(), [i](Value const& p) {
					return p.id == i;
				});
				if (it!=descriptor.value.end())
				{
					drawPoint(x.value[i], (*it));
				}
				else
				{
					drawPoint(x.value[i], 1.);
				}
			}
		}
	}
	else
	{
		assert(inlet=1);
		addDescriptor(x);
	}
}

void Renderer3d::operator()(Value const& x, int inlet)
{
	if (inlet==1)
	{
		addDescriptor(x);
	}
		
}

void Renderer3d::operator()(ListValue const& x, int inlet)
{
	if (inlet==1)
	{
		addDescriptor(x);
	}
}


void Renderer3d::addDescriptor(Data const& x)
{
	cleanDescriptors(x.timestamp());
	if (x.type() & (POINT3D | SKELETON3D | SCENE3D | LIST_POINT3D | VALUE | LIST_VALUE))
	{
		mDescriptors.push_front(x);
	}
}


void Renderer3d::cleanDescriptors(double latestTimestamp)
{
	while (!mDescriptors.empty() && mDescriptors.back().timestamp() - 2 > latestTimestamp)
	{
		mDescriptors.pop_back();
	}
	/// vague sanity check against memory leak
	assert(mDescriptors.size() < 100000);
}


Data const& Renderer3d::getDescriptor(int id, Types types)
{
	static Data nullData;
	for (Data const& d: mDescriptors)
	{
		if (d.id()==id && (d.type() & types))
		{
			return d;
		}
	}
	return nullData;
}




// Renderer2d

Renderer2d::Renderer2d()
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

void Renderer2d::operator()(Image2d const& v, int inlet)
{
	setupMatricesWindow();
	gl::draw(gl::Texture(v.toSurface()));
}

void Renderer2d::operator()(Point2d const& p, int inlet, int count)
{
	if (inlet==0)
	{
		setupMatricesWindow();
		gl::color(ColorA(1.f, 1.f, 1.f, 0.9f));
		double size = 2.5 * mValueScale;
		hm_debug(elapsedTime() - mLastValues.timestamp);
		if (!mLastValues.value.empty() && elapsedTime() - mLastValues.timestamp < mValueMemoryDuration)
		{
			size *= mLastValues.value[std::min<int>(count, mLastValues.value.size()-1)];
		}
		gl::drawSolidCircle(p.value, size);
	}
}


void Renderer2d::operator()(ListPoint2d const& x, int inlet)
{
	if (inlet==0)
	{
		for (int i=0; i<x.value.size(); i++)
		{
			(*this)(x.value[i], inlet, i);
		}
	}
}


void Renderer2d::operator()(Value const& x, int inlet)
{
	if (inlet==1)
	{
		mLastValues.value.assign(1, x);
		mLastValues.timestamp = x.timestamp;
	}
}


void Renderer2d::operator()(ListValue const& x, int inlet)
{
	if (inlet==1)
	{
		mLastValues = x;
	}
}






