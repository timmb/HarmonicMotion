/// Harmonic Motion
/// Classes for rendering data
#pragma once
#include "Common.h"
#include "Data.h"
#include "cinder/Area.h"
#include "cinder/Color.h"
#include "Parameter.h"

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
		/// \param inlet indicates the inlet of nodeWidget into which the
		/// data arrived		
		void render(Data const& data, ci::Area const& viewport, int inlet=0);
		
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
			/// A user-visible descriptive name of the parameter
			std::string name;
			// See Parameter class for description of these properties
			double softMin;
			double softMax;
			double hardMin;
			double hardMax;
			
			ParameterDescription(ParameterValuePointerContainer const& pointer_, std::string const& name_)
			: pointer(pointer_)
			, name(name_)
			, softMin(0)
			, softMax(100)
			, hardMin(-9999999)
			, hardMax(9999999)
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
	
	/// Renderer for 3D points as circles
	class RendererBlob : public Renderer
	{
	public:
		RendererBlob();
		
		virtual void operator()(Point3d const& x, int inlet, ci::ColorA const& color=ci::ColorA::white());
		virtual void operator()(Skeleton3d const& x, int inlet) override;
		virtual void operator()(Scene3d const& x, int inlet) override;
		virtual void operator()(ListPoint3d const& x, int inlet) override;
		
	private:
	};
	
	/// Renderer for 2D images
	class Renderer2D : public Renderer
	{
	public:
		Renderer2D();
		
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
}


