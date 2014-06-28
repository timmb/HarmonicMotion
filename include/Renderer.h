/// Harmonic Motion
/// Classes for rendering data
#pragma once
#include "Common.h"
#include "Data.h"
#include "cinder/Area.h"
#include "cinder/Color.h"
#include "Parameter.h"
#include <deque>
#include <boost/circular_buffer.hpp>

namespace hm
{
	class Renderer;
	typedef std::shared_ptr<Renderer> RendererPtr;
	
	/// Base type for all renderers
	class Renderer
	{
	public:
		struct InletDescription;
		struct ParameterDescription;
		
		/// \param name User-visible name of this renderer.
		/// \param description User-visible description of what this renderer
		/// does with data.
		/// \param numInlets How many inlets this renderer requires.
		/// \param inlets A set of InletDescription structs describing the inlets
		/// required by this renderer
		Renderer(std::string const& name,
				 std::string const& description,
				 std::vector<InletDescription> const& inlets);
		
		/// Renders data to the current OpenGL context.
		/// \param dataSet Is a list of Data objects that are to be
		/// rendered, alongside the inlet into which they arrived.
		void render(std::vector<std::pair<Data, int>> dataSet, ci::Area const& viewport);
		
		/// This function iterates through the dataset and uses the
		/// operator() functions below to render it. By default, data
		/// is renderered from the highest numbered inlet to the lowest,
		/// with multiple Data at each inlet rendering in the order:
		/// Images, 3d vector types, 2d vector types, 1d types.
		/// Derived classes can override this function if they would
		/// like a different order
		virtual void render(std::vector<std::pair<Data, int>> dataSet);
		
//		/// If you are using multiple renderers in a single context
//		/// then they may set up the environment matrices differently.
//		/// Use this function to request that this instance reinstates
//		/// its setup of the environment matrices before the next render.
//		void refresh();
		
		// Derived types should override datatypes that they can render
		
		virtual void operator()(DataNull const& x, int inlet) {}
		virtual void operator()(Value const& x, int inlet) {}
		virtual void operator()(Point2d const& x, int inlet) {}
		virtual void operator()(Point3d const& x, int inlet) {}
		virtual void operator()(Skeleton3d const& x, int inlet) {}
		virtual void operator()(Scene3d const& x, int inlet) {}
		virtual void operator()(Image2d const& x, int inlet) {}
		virtual void operator()(ListValue const& x, int inlet) {}
		virtual void operator()(ListPoint2d const& x, int inlet) {}
		virtual void operator()(ListPoint3d const& x, int inlet) {}
		
		typedef void result_type;

		std::string const name;
		std::string const description;
		/// A list of inlets required by this renderer
		std::vector<InletDescription> const inlets;
		/// Renderers may have parameters, which should be created in
		/// this function which is called by NodeRenderer.
		std::vector<ParameterDescription> parameters() const;
		/// When a renderer is constructed, the NodeRenderer provides it with
		/// the constructed parameters that were requested through
		/// \c setParameterDescriptions in case they need to access them
		virtual void provideParameters(std::vector<ParameterPtr> const& parameters) {}
		
		
		/// Describes an inlet that might be requested by a Renderer
		struct InletDescription
		{
			/// A descriptive name for the inlet.
			std::string name;
			/// The types the inlet is expecting.
			Types types;
			
			InletDescription(std::string const& name_, Types types_)
			: name(name_)
			, types(types_)
			{}
		};
		
		/// Describes a parameter that might be requested by a Renderer
		struct ParameterDescription
		{
			/// A pointer to the value to be controlled by the parameter
			ParameterValuePointerContainer pointer;
			/// A user-friendly name for the parameter
			std::string name;
			/// A user-friendly description fo the parameter
			std::string description;
			// See Parameter class for description of these properties
			double softMin;
			double softMax;
			double hardMin;
			double hardMax;
			
			ParameterDescription(ParameterValuePointerContainer const& pointer_, std::string const& name_, std::string const& description_="", double softMin_=0, double softMax_=100, double hardMin_=-9999999, double hardMax_=9999999)
			: pointer(pointer_)
			, name(name_)
			, description(description_)
			, softMin(softMin_)
			, softMax(softMax_)
			, hardMin(hardMin_)
			, hardMax(hardMax_)
			{}
		};
		
	protected:
		// If you are not using these provided setup functions then you
		// should check mNeedsRefresh before each render call and re-setup
		// the projection/modelview matrices if it is true (and then set it
		// to false).
		
		/// Set up the camera for the scene described in the \p sceneMeta
		/// of the most recent Data to have arrived. y axis goes up.
		/// \note This function will only have any impact if mNeedsRefresh is false
		/// and will set mNeedsRefresh to false after it is applied. Set
		/// mNeedsRefresh to true to force it.
		void setupMatrices3d();
		/// Set up the camera for a normal 2D window environment. y axis
		/// goes down.
		/// \note This function will only have any impact if mNeedsRefresh is false
		/// and will set mNeedsRefresh to false after it is applied. Set
		/// mNeedsRefresh to true to force it.
		void setupMatricesWindow();
		/// Set the parameter descriptions requested of this node. This
		/// must be called in the constructor to have an effect.
		void setParameterDescriptions(std::vector<ParameterDescription> const& descriptions);

		ci::Area mViewport;
		/// For derived types in a render function this will always point to
		/// the current SceneMeta
		SceneMetaPtr mLastSceneMeta;
		
		/// Derived types should call this to test if they need to
		/// setup the projection/modelview matrices before the current
		/// render, and set it to false once they have set them up.
		bool mNeedsRefresh;
		
	private:
		std::string mName;
		std::string mDescription;
		std::vector<ParameterDescription> mParameterDescriptions;
	};
	
//	/// The default renderer takes a pick of other renderers
//	class RendererDefault : public Renderer
//	{
//	public:
//		RendererDefault();
//		
//		
//	};
	
	/// Renderer for 3D data
	/// Positional data is drawn as:
	/// - Skeleton3d/Scene3d: lines connecting joints. Colour based on confidence
	/// - Point3d / List3D: blobs
	/// Non-positional data is drawn as:
	/// - Skeleton3d/Scene3d/Point3d: 3D vectors sticking out of joints, where IDs match
	/// - Value / ListValue: Blobs of this size on joints, where IDs match
	class Renderer3d : public Renderer
	{
	public:
		Renderer3d();
		
		virtual void operator()(Point3d const& x, int inlet) override;
		virtual void operator()(Skeleton3d const& x, int inlet) override;
		virtual void operator()(Scene3d const& x, int inlet) override;
		virtual void operator()(ListPoint3d const& x, int inlet) override;
		virtual void operator()(Value const& x, int inlet) override;
		virtual void operator()(ListValue const& x, int inlet) override;
		
	private:
		void drawPoint(Point3d const& x, Point3d const& descriptor);
		void drawPoint(Point3d const& x, Value const& descriptor);
		void drawSkeleton(Skeleton3d const& x, Skeleton3d const& descriptor);
		void drawSkeleton(Skeleton3d const& x, ListValue const& descriptor);
		/// Basic skeleton
		void drawSkeletonLines(Skeleton3d const& x);
		
		/// Data that has arrived in the second inlet that will describe
		/// how data in the first inlet will be rendered. New data is put at
		/// front of this queue. Old data is removed from the back when
		/// we receive any data that is more than 2 seconds newer than
		/// it.
		std::deque<Data> mDescriptors;
		
		/// Add a descriptor to the queue
		void addDescriptor(Data const& x);
		/// Clear up old descriptors from the queue more than a second
		/// old given \p latestTimestamp
		void cleanDescriptors(double latestTimestamp);
		/// Returns the newest descriptor matching the Types and id.
		/// \note This does not search the id's of elements inside a container
		/// (so searching for a SKELETON3D with id of 1 will not search inside
		/// a scene for that skeleton)
		/// \return A descriptor matching the description, or a reference to
		/// DataNull if none is found
		Data const& getDescriptor(int id, Types possibleDescriptorTypes);
	};
	
	/// Renderer for 2D images
	class Renderer2d : public Renderer
	{
	public:
		Renderer2d();
		
		virtual void operator()(Image2d const& x, int inlet) override;
		virtual void operator()(Point2d const& x, int inlet, int count=0);
		virtual void operator()(ListPoint2d const& x, int inlet) override;
		
		virtual void operator()(Value const& x, int inlet) override;
		virtual void operator()(ListValue const& x, int inlet) override;
	
	private:
		ListValue mLastValues;
		/// How long in seconds until the last value received is forgotten
		/// and a default value is used instead.
		double mValueMemoryDuration;
		/// Values are scaled by this amount
		double mValueScale;
	};
	
	class RendererHistory : public Renderer
	{
	public:
		RendererHistory();
		
		virtual void operator()(Value const& x, int inlet) override;
		
		virtual void provideParameters(std::vector<ParameterPtr> const& parameters) override;
		
	private:
		void callbackAutoBounds();
		
		bool mAutoBounds;
		int mSize;
		double mLowerBound;
		double mUpperBound;
		boost::circular_buffer<Data> mBuffer;
		std::vector<ParameterPtr> mParametersToHideForAutoBounds;
	};
}






