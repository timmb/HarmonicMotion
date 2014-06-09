/// Harmonic Motion
/// Classes for rendering data
#pragma once
#include "Common.h"
#include "Data.h"
#include "cinder/Area.h"
#include "cinder/Color.h"

namespace hm
{
	class Renderer;
	typedef std::shared_ptr<Renderer> RendererPtr;
	
	/// Base type for all renderers
	class Renderer
	{
	public:
		Renderer(std::string const& name,
				 std::string const& description);
		
		/// Renders data to the current OpenGL context.
		void render(Data const& data, ci::Area const& viewport);
		std::string name() const { return mName; }
		std::string description() const { return mDescription; }
		
		/// If you are using multiple renderers in a single context
		/// then they may set up the environment matrices differently.
		/// Use this function to request that this instance reinstates
		/// its setup of the environment matrices before the next render.
		void refresh();
		
		// Derived types should override datatypes that they can render
		
		virtual void operator()(DataNull const& x) {}
		virtual void operator()(Value const& x) {}
		virtual void operator()(Point3d const& x) {}
		virtual void operator()(Skeleton3d const& x) {}
		virtual void operator()(Scene3d const& x) {}
		virtual void operator()(Image2d const& x) {}
		
		typedef void result_type;
		
	protected:
		/// Set up the camera for the scene described in the \p sceneMeta
		/// of the most recent Data to have arrived. y axis goes up.
		void setupMatrices3d();
		/// Set up the camera for a normal 2D window environment. y axis
		/// goes down.
		void setupMatricesWindow();

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
	};
	
	/// Renderer for 3D points as circles
	class RendererBlob : public Renderer
	{
	public:
		RendererBlob();
		
		virtual void operator()(Point3d const& x, ci::ColorA const& color=ci::ColorA::white());
		virtual void operator()(Skeleton3d const& x) override;
		virtual void operator()(Scene3d const& x) override;
		
	private:
	};
	
	/// Renderer for 2D images
	class Renderer2D : public Renderer
	{
	public:
		Renderer2D();
		
		virtual void operator()(Image2d const& x);
	};
}


