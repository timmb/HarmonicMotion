#pragma once

#include "BaseData.h"
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "cinder/Surface.h"
#include "Value.h"

namespace hm
{
	class Image2d : public BaseData
	{
	public:
		typedef BaseData BaseType;
		
		/// Construct a black 640x480 RGB image
		explicit
		Image2d(double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta=SceneMeta::sDefaultSceneMeta);
		
		/// Construct a black RGB image of the given size
		Image2d(int width, int height, double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta=SceneMeta::sDefaultSceneMeta);
		
		/// Construct an image matching the given \p surface
		explicit
		Image2d(ci::Surface8u surface, double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta=SceneMeta::sDefaultSceneMeta);
		
		/// Construct an image matching the given \p matrix.
		/// \param matrix will be allocated to 640,480 RGB 8 bit if it
		/// is not already allocated.
		explicit
		Image2d(cv::Mat matrix, double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta=SceneMeta::sDefaultSceneMeta);
		
		/// Data representing this image.
		/// \invariant value.rows > 0 && value.cols > 0
		cv::Mat value;
		
		/// \return value as a ci::Surface8u. This shares data with this Image2d
		/// object and will be invalid once this Image2d object is destroyed.
		ci::Surface8u toSurface() const;
		
		virtual std::ostream& printTo(std::ostream& out) const override;
		virtual Type type() const override { return IMAGE2D; }
		
		Image2d operator*(Value const& rhs) const;
		Image2d operator/(Value const& rhs) const;
		Image2d operator+(Value const& rhs) const;
		Image2d operator-(Value const& rhs) const;
		Image2d& operator*=(Value const& rhs);
		Image2d& operator/=(Value const& rhs);
		Image2d& operator+=(Value const& rhs);
		Image2d& operator-=(Value const& rhs);
		

		// Point-wise multiplication / division. If input images are of
		// different size then the larger image will be cropped
		Image2d operator*(Image2d const& rhs) const;
		Image2d operator/(Image2d const& rhs) const;
		Image2d operator+(Image2d const& rhs) const;
		Image2d operator-(Image2d const& rhs) const;
		Image2d& operator*=(Image2d const& rhs);
		Image2d& operator/=(Image2d const& rhs);
		Image2d& operator+=(Image2d const& rhs);
		Image2d& operator-=(Image2d const& rhs);
		
		Image2d operator+() const;
		Image2d operator-() const;
		
		bool operator==(Image2d const& rhs) const;
		bool operator!=(Image2d const& rhs) const;
		
		
		/// Checks that \c value is not a null image.
		bool invariant() const;
		
	private:
		void checkValueHasDimensions();
	};
	
	hm_data_define_free_scalar_operators(Image2d)
	
}




