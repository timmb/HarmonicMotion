#pragma once

#include "BaseData.h"
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "cinder/Surface.h"
#include "Value.h"
#include "Common.h"
#include "CinderOpenCV.h"

namespace hm
{
	class Image2d : public BaseData
	{
	public:
		typedef BaseData BaseType;

		/// Images can optionally have a role set for them if they do not represent typical image data
		enum Role
		{
			NORMAL, ///< The image is a normal image captured from a camera
			MASK, /// The image is a one channel 8 bit mask with true as 255 and false as 0
			DEPTH, ///< Each pixel value indicates a distance from the camera
			USER_INDEX, ///< Each pixel value indicates a user ID counting from 0 and should be drawn with a different colour
			FLOW_FIELD, ///< Each pixel represents a two dimension vector used in, e.g., an optical flow calculation. value must be two channels
		};

		/// Construct a black 640x480 RGB image
		explicit
			Image2d(double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta = SceneMeta::defaultSceneMeta(), Role role=NORMAL);

		/// Construct a black RGB image of the given size
		Image2d(int width, int height, double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta = SceneMeta::defaultSceneMeta(), Role role = NORMAL);

		/// Construct an image matching the given \p surface
		explicit
			Image2d(ci::Surface8u surface, double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta = SceneMeta::defaultSceneMeta(), Role role = NORMAL);

		/// Construct an image matching the given \p channel
		explicit
			Image2d(ci::Channel8u channel, double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta = SceneMeta::defaultSceneMeta(), Role role = NORMAL);

		/// Construct an image matching the given \p channel
		explicit
			Image2d(ci::Channel16u channel, double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta = SceneMeta::defaultSceneMeta(), Role role = NORMAL);

		/// Construct an image matching the given \p matrix.
		/// \param matrix will be allocated to 640,480 RGB 8 bit if it
		/// is not already allocated.
		explicit
			Image2d(cv::Mat matrix, double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta = SceneMeta::defaultSceneMeta(), Role role = NORMAL);

		/// Copy constructor: the underlying data is cloned. Use Image2d(cv::Mat...) to share data.
		Image2d(Image2d const& rhs);
		Image2d& operator=(Image2d const& rhs);

		/// Data representing this image.
		/// \invariant value.rows > 0 && value.cols > 0
		cv::Mat value;
		/// \see Role
		Role role;

		/// \return value as a ci::Surface8u. This shares data with this Image2d
		/// object and will be invalid once this Image2d object is destroyed.
		template <typename T>
		ci::SurfaceT<T> toSurface() const;

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


	inline Image2d::Role chooseRole(Image2d const& x, Image2d const& y)
	{
		return (x.role == Image2d::NORMAL) ? y.role : x.role;
	}


	// Helper:
#define _hm_image2d_define_free_single_scalar_op(Type, op, Scalar) \
	inline Type operator op (Scalar lhs, Type rhs) \
	{ \
	rhs.value = lhs op rhs.value; \
	return rhs; \
	}

	// Helper:
#define _hm_image2d_define_free_scalar_op(Type, op) \
	_hm_image2d_define_free_single_scalar_op(Type, op, float) \
	_hm_image2d_define_free_single_scalar_op(Type, op, double) \
	inline Type operator op (Value const& lhs, Type rhs) \
	{ \
	rhs.value = lhs.value op rhs.value; \
	rhs.timestamp = chooseTimestamp(lhs, rhs); \
	rhs.id = chooseId(lhs, rhs); \
	rhs.sceneMeta = chooseSceneMeta(lhs, rhs); \
	rhs.role = rhs.role; \
	return rhs; \
	}

#define hm_image2d_define_free_mult_scalar_operators(Type) \
	_hm_image2d_define_free_scalar_op(Type, *) \
	_hm_image2d_define_free_scalar_op(Type, / )

#define hm_image2d_define_free_scalar_operators(Type) \
	_hm_image2d_define_free_scalar_op(Type, +) \
	_hm_image2d_define_free_scalar_op(Type, -) \
	_hm_image2d_define_free_scalar_op(Type, *) \
	_hm_image2d_define_free_scalar_op(Type, / )

	hm_image2d_define_free_scalar_operators(Image2d);

	template <typename T>
	ci::SurfaceT<T> Image2d::toSurface() const
	{
		cv::Mat m(value);
		return ci::SurfaceT<T>(ci::fromOcv(m));
	}



}




