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

namespace hm {

	using namespace ci;
	using namespace std;
	using boost::assign::list_of;
	using boost::assign::map_list_of;
	using boost::adaptors::reversed;

	Renderer::Renderer(std::string const& name_, std::string const& description_, vector<InletDescription> const& inlets_)
		: name(name_)
		, description(description_)
		, inlets(inlets_)
		, mLastSceneMeta(SceneMeta::defaultSceneMeta())
		, mNeedsRefresh(true)
		, mIsGlInitialized(false)
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
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POLYGON_SMOOTH);
		}
	}

	void Renderer::setupMatricesWindow()
	{
		if (mNeedsRefresh || mLastSceneMeta->viewportSize != mLastSceneMetaViewportSize)
		{
			mLastSceneMetaViewportSize = mLastSceneMeta->viewportSize;
			mLastSceneMeta->setupWindowLetterbox(mViewport.getWidth(), mViewport.getHeight());
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POLYGON_SMOOTH);
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
		if (!mIsGlInitialized)
		{
			initializeGl();
			mIsGlInitialized = true;
		}
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
		static_assert((IMAGE2D | SCENE3D | SKELETON3D | LIST_POINT3D | POINT3D | LIST_POINT2D | POINT2D | LIST_VALUE | VALUE | UNDEFINED) == ALL_TYPES, "Renderer type order is not defined for all types");
		// Comparer function to sort dataSet
		static auto comparer = [](pair<Data, int> x, pair<Data, int> y) {
			// x happens first if inlet is higher, or inlets are same and type is
			// earlier in the typeOrder list
			return x.second > y.second || (x.second == y.second && typeOrder[x.first.type()] < typeOrder[y.first.type()]);
		};

		std::sort(begin(dataSet), end(dataSet), comparer);
		if (!dataSet.empty())
		{
			mLastSceneMeta = dataSet[0].first.sceneMeta();
		}
		for (auto dataPair : dataSet)
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
		: Renderer("3D Renderer", "Renderer for 3D data such as Skeletons and Scenes.",
		list_of
		(InletDescription("3D points received here will be rendered as blobs.", POINT3D | SKELETON3D | SCENE3D | LIST_POINT3D))
		(InletDescription("3D or Value data received here will modify how points delivered to the first inlet are rendered. For example a Scene3d here will cause lines to stick out of the joints of a Scene3d provided in the first inlet.", POINT3D | SKELETON3D | SCENE3D | LIST_POINT3D | VALUE | LIST_VALUE))
		)
	{

	}

	void Renderer3d::operator()(Point3d const& x, int inlet)
	{
		if (inlet == 0)
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
		gl::drawSphere(x.value, 0.05f, 12);
		gl::drawLine(x.value, x.value + descriptor.value * 4);
	}

	void Renderer3d::drawPoint(Point3d const& x, Value const& descriptor)
	{
		gl::drawSphere(x.value, 0.05*descriptor.value);
	}

	void Renderer3d::operator()(Skeleton3d const& x, int inlet)
	{
		if (inlet == 0)
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
			assert(inlet == 1);
			addDescriptor(x);
		}
	}

	void Renderer3d::drawSkeleton(Skeleton3d const& x, Skeleton3d const& descriptor)
	{
		drawSkeletonLines(x);
		gl::color(ColorA::white());
		for (int i = 0; i < NUM_JOINTS; i++)
		{
			drawPoint(x.joint(i), descriptor.joint(i));
		}
	}

	void Renderer3d::drawSkeleton(Skeleton3d const& x, ListValue const& descriptor)
	{
		drawSkeletonLines(x);
		gl::color(ColorA::white());
		for (int i = -0; i < NUM_JOINTS; i++)
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
		glPushAttrib(GL_LINE_WIDTH);
		glLineWidth(2.5);
		for (int i = 0; i < NUM_LIMBS; i++)
		{
			float startBrightness = x.jointConfidence(limbJoints(i).first) * 0.5f + 0.5f;
			float endBrightness = x.jointConfidence(limbJoints(i).second) * 0.5f + 0.5f;
			bool isRight = limbSide(i) == RIGHT;
			bool isLeft = limbSide(i) == LEFT;
			ColorA startColor(0.2f + 0.8f*int(isRight)*startBrightness,
				0.5f * startBrightness,
				0.2f + 0.8f*int(isLeft)*startBrightness,
				1.0f);
			ColorA endColor(0.2f + 0.8f*int(isRight)*endBrightness,
				0.5f * endBrightness,
				0.2f + 0.8f*int(isLeft)*endBrightness,
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
		glPopAttrib();
	}


	void Renderer3d::operator()(Scene3d const& x, int inlet)
	{
		if (inlet == 0)
		{
			cleanDescriptors(x.timestamp);
			setupMatrices3d();
			Data descriptor_ = getDescriptor(x.id, SCENE3D);
			static Scene3d emptyScene3d;
			// if we don't find a descriptor then use an empty Scene3d
			Scene3d const& descriptor = descriptor_.isScene3d() ? descriptor_.asScene3d() : emptyScene3d;
			for (int i = 0; i < x.value.size(); i++)
			{
				auto it = find_if(descriptor.value.begin(), descriptor.value.end(), [i](Skeleton3d const& s){
					return s.id == i;
				});
				if (it != descriptor.value.end())
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
			assert(inlet = 1);
			addDescriptor(x);
		}
	}

	void Renderer3d::operator()(ListPoint3d const& x, int inlet)
	{
		if (inlet == 0)
		{
			cleanDescriptors(x.timestamp);
			Data descriptor_ = getDescriptor(x.id, LIST_POINT3D | LIST_VALUE);
			if (descriptor_.isListPoint3d())
			{
				ListPoint3d const& descriptor = descriptor_.asListPoint3d();
				for (int i = 0; i < x.value.size(); i++)
				{
					auto it = find_if(descriptor.value.begin(), descriptor.value.end(), [i](Point3d const& p) {
						return p.id == i;
					});
					if (it != descriptor.value.end())
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
				ListValue const& descriptor = descriptor_.isListValue() ? descriptor_.asListValue() : emptyListValue;
				for (int i = 0; i < x.value.size(); i++)
				{
					auto it = find_if(descriptor.value.begin(), descriptor.value.end(), [i](Value const& p) {
						return p.id == i;
					});
					if (it != descriptor.value.end())
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
			assert(inlet = 1);
			addDescriptor(x);
		}
	}

	void Renderer3d::operator()(Value const& x, int inlet)
	{
		if (inlet == 1)
		{
			addDescriptor(x);
		}

	}

	void Renderer3d::operator()(ListValue const& x, int inlet)
	{
		if (inlet == 1)
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
		while (!mDescriptors.empty() && latestTimestamp - 2 > mDescriptors.back().timestamp())
		{
			mDescriptors.pop_back();
		}
		/// vague sanity check against memory leak
		assert(mDescriptors.size() < 100000);
	}


	Data const& Renderer3d::getDescriptor(int id, Types types)
	{
		static Data nullData;
		for (Data const& d : mDescriptors)
		{
			if (d.id() == id && (d.type() & types))
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
		, mTimeOfLastRecompile(-42.f)
		, mFlowFieldWindowSize(4)
		, mFlowFieldScale(1.0)
		, mCycle16BitImages(true)
	{
		vector<ParameterDescription> parameters;
		{
			auto p = ParameterDescription(&mValueScale, "Scale values by");
			parameters.push_back(p);

			p = ParameterDescription(&mFlowFieldWindowSize, "Flow field window size", "When rendering a flow field (an image of vectors), this determines how big a square of pixels are averaged over to render a single vector on the image.", 1, 100, 1, 99999999);
			parameters.push_back(p);

			p = ParameterDescription(&mFlowFieldScale, "Flow field scale", "When renderering a flow field, drawn vectors are scaled by this amount.");
			parameters.push_back(p);

			p = ParameterDescription(&mCycle16BitImages, "Cycle 16 bit images", "When rendering 16 bit images, draw cycles of greyscale to reveal all the depth of the image, rather than reducing it down to an 8 bit greyscale image for the screen.");
			parameters.push_back(p);
		}
		setParameterDescriptions(parameters);
	}


	void Renderer2d::initializeGl()
	{
		// TODO: Bake these shaders into the source code
		//registerShader(mUserIndexShader, &mUserIndexShaderIsCompiled, "../../resources/shaders/v_TexturePassthrough.glsl", "../../resources/shaders/f_UserIndexTexture.glsl");
		compileShader();
	}

	void Renderer2d::compileShader()
	{
		mUserIndexShaderIsCompiled = false;
		try
		{
			mUserIndexShader = gl::GlslProg::create(ci::loadFile("../../resources/shaders/v_TexturePassthrough.glsl"), ci::loadFile("../../resources/shaders/f_UserIndexTexture.glsl"));
			mUserIndexShaderIsCompiled = true;
		}
		catch (gl::GlslProgCompileExc const& e)
		{
			hm_error("Failed to compile User Index Texture shader: " << e.what());
		}
		mTimeOfLastRecompile = elapsedTime();
	}

	// borrowed from KinectCommonBridge
	Channel8u channel16To8(const Channel16u& channel, uint8_t bytes = 4)
	{
		Channel8u channel8;
		if (channel) {
			channel8 = Channel8u(channel.getWidth(), channel.getHeight());
			Channel16u::ConstIter iter16 = channel.getIter();
			Channel8u::Iter iter8 = channel8.getIter();
			while (iter8.line() && iter16.line()) {
				while (iter8.pixel() && iter16.pixel()) {
					iter8.v() = static_cast<uint8_t>(iter16.v() >> bytes);
				}
			}
		}
		return channel8;
	}

	void Renderer2d::operator()(Image2d const& v, int inlet)
	{
		if (gRequestShaderRecompile > mTimeOfLastRecompile)
		{
			compileShader();
			mTimeOfLastRecompile = gRequestShaderRecompile;
		}
		setupMatricesWindow();
		if (v.role == Image2d::USER_INDEX && mUserIndexShaderIsCompiled)
		{
			mUserIndexShader->bind();
			gl::Texture::Format format;
			format.setMagFilter(GL_NEAREST);
			format.setMinFilter(GL_NEAREST);
			gl::Texture tex(v.toSurface<uint8_t>(), format);
			tex.bind();
			mUserIndexShader->uniform("tex", 0);
			gl::drawSolidRect(tex.getBounds(), false);
			mUserIndexShader->unbind();
		}
		else if (v.value.depth() == CV_16U && v.value.channels() == 1)
		{
			gl::draw(gl::Texture(channel16To8(v.toSurface<uint16_t>().getChannel(0), mCycle16BitImages? 0 : 4)));
		}
		else if (v.role == Image2d::FLOW_FIELD)
		{
			if (v.value.channels() < 2)
			{
				hm_warning("Unable to render flow field as only two channels found.");
			}
			for (int y = 0; y < v.value.rows; y += mFlowFieldWindowSize)
			{
				for (int x = 0; x < v.value.cols; x += mFlowFieldWindowSize)
				{
					cv::Scalar mean = cv::mean(v.value(
						cv::Range(y, min(v.value.rows, y + mFlowFieldWindowSize)),
						cv::Range(x, min(v.value.cols, x + mFlowFieldWindowSize))));
					float brightness = mean.dot(mean) / (mFlowFieldWindowSize*mFlowFieldWindowSize);
					brightness = 0.5f + 0.5f * max(0.f, min(1.f, brightness));
					gl::color(ColorA(brightness, brightness, brightness, 1.0));
					ci::gl::drawLine(Vec3f(x, y, 0), Vec3f(x, y, 0) + Vec3f(mean[0], mean[1], 0) * mFlowFieldScale);
				}
			}
		}
		else
		{
			try
			{
				gl::draw(gl::Texture(v.toSurface<uint8_t>()));
			}
			catch (ImageIoExceptionIllegalColorModel const& e)
			{
				hm_error("Could not render image: " << e.what());
			}
		}
	}

	void Renderer2d::operator()(Point2d const& p, int inlet, int count)
	{
		if (inlet == 0)
		{
			setupMatricesWindow();
			gl::color(ColorA(1.f, 1.f, 1.f, 0.9f));
			double size = 2.5 * mValueScale;
			hm_debug(elapsedTime() - mLastValues.timestamp);
			if (!mLastValues.value.empty() && elapsedTime() - mLastValues.timestamp < mValueMemoryDuration)
			{
				size *= mLastValues.value[std::min<int>(count, mLastValues.value.size() - 1)];
			}
			gl::drawSolidCircle(p.value, size);
		}
	}


	void Renderer2d::operator()(ListPoint2d const& x, int inlet)
	{
		if (inlet == 0)
		{
			for (int i = 0; i < x.value.size(); i++)
			{
				(*this)(x.value[i], inlet, i);
			}
		}
	}


	void Renderer2d::operator()(Value const& x, int inlet)
	{
		if (inlet == 1)
		{
			mLastValues.value.assign(1, x);
			mLastValues.timestamp = x.timestamp;
		}
	}


	void Renderer2d::operator()(ListValue const& x, int inlet)
	{
		if (inlet == 1)
		{
			mLastValues = x;
		}
	}


	///// ------------------------


	RendererHistory::RendererHistory()
		: Renderer("History Renderer", "Renders Value data as a graph with a history.",
		list_of(InletDescription("Values to render", VALUE)))
		, mAutoBounds(false)
		, mSize(50)
		, mLowerBound(-1)
		, mUpperBound(1)
		, mBuffer(mSize)
	{
		setParameterDescriptions
			(list_of
			(ParameterDescription(&mSize,
			"Buffer size",
			"How many recent values to draw",
			1,
			10000,
			1,
			100000))
			(ParameterDescription(&mAutoBounds,
			"Automatically set bounds",
			"Automatically choose the bounds of the graph based on the highest and lowest value it needs to display",
			0, 1, 0, 1))
			(ParameterDescription(&mLowerBound,
			"Lower bound",
			"The lowest value to fit onto the graph",
			-100,
			100))
			(ParameterDescription(&mUpperBound,
			"Upper bound",
			"The highest value to fit onto the graph",
			-100,
			100))
			);
	}

	void RendererHistory::provideParameters(std::vector<ParameterPtr> const& parameters)
	{
		bool callbackSet = false;
		for (ParameterPtr p : parameters)
		{
			if (p->name() == "Lower bound" || p->name() == "Upper bound")
			{
				mParametersToHideForAutoBounds.push_back(p);
			}
			else if (p->name() == "Automatically set bounds")
			{
				p->addNewExternalValueCallback([this](){ callbackAutoBounds(); });
				callbackSet = true;
			}
		}
		assert(callbackSet);
		assert(mParametersToHideForAutoBounds.size() == 2);
	}

	void RendererHistory::callbackAutoBounds()
	{
		for (ParameterPtr p : mParametersToHideForAutoBounds)
		{
			p->setVisible(!mAutoBounds);
		}
	}



	void RendererHistory::operator()(Value const& x, int inlet)
	{
		assert(inlet == 0);
		if (mBuffer.capacity() != mSize)
		{
			assert(mSize > 0);
			assert(mSize <= 100000);
			mBuffer.set_capacity(mSize);
		}
		mBuffer.push_back(x);
		if (mNeedsRefresh)
		{
			gl::setMatricesWindow(mViewport.getSize(), false);
			mNeedsRefresh = false;
		}
		// find limits
		float min(std::numeric_limits<float>::max()), max(-std::numeric_limits<float>::max());
		if (mAutoBounds)
		{
			for (Data const& d : mBuffer)
			{
				assert(d.isValue());
				float value = d.asValue().value;
				min = std::min(min, value);
				max = std::max(max, value);
			}
		}
		else
		{
			min = mLowerBound;
			max = mUpperBound;
		}
		glPushAttrib(GL_LINE_WIDTH);
		glLineWidth(2.0f);
		gl::color(ColorA(1.f, 1.f, 1.f, 0.7f));
		float step = mViewport.getWidth() / float(mSize);
		float t = 0.f;
		float scale = mViewport.getHeight() / (max - min);
		glBegin(GL_LINE_STRIP);
		for (Data const& d : mBuffer)
		{
			assert(d.isValue());
			Vec2f v(t, (float(d.asValue().value) - min) * scale);
			gl::vertex(v);
			t += step;
		}
		glEnd();
		glPopAttrib();
	}



}











