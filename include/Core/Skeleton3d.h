#pragma once
#include "Common.h"
#include <cassert>
#include <vector>
#include "BaseData.h"
#include "Point3d.h"
#include "SceneMeta.h"

// OpenNI's values:
//XN_SKEL_HEAD			= 1,
//XN_SKEL_NECK			= 2,
//XN_SKEL_TORSO			= 3,
//XN_SKEL_WAIST			= 4,
//
//XN_SKEL_LEFT_COLLAR		= 5,
//XN_SKEL_LEFT_SHOULDER	= 6,
//XN_SKEL_LEFT_ELBOW		= 7,
//XN_SKEL_LEFT_WRIST		= 8,
//XN_SKEL_LEFT_HAND		= 9,
//XN_SKEL_LEFT_FINGERTIP	=10,
//
//XN_SKEL_RIGHT_COLLAR	=11,
//XN_SKEL_RIGHT_SHOULDER	=12,
//XN_SKEL_RIGHT_ELBOW		=13,
//XN_SKEL_RIGHT_WRIST		=14,
//XN_SKEL_RIGHT_HAND		=15,
//XN_SKEL_RIGHT_FINGERTIP	=16,
//
//XN_SKEL_LEFT_HIP		=17,
//XN_SKEL_LEFT_KNEE		=18,
//XN_SKEL_LEFT_ANKLE		=19,
//XN_SKEL_LEFT_FOOT		=20,
//
//XN_SKEL_RIGHT_HIP		=21,
//XN_SKEL_RIGHT_KNEE		=22,
//XN_SKEL_RIGHT_ANKLE		=23,
//XN_SKEL_RIGHT_FOOT		=24

namespace hm
{
	/// Joints are referred to by these indexes.
	/// NB these are different values from the OpenNI indexes
	/// to make it easy to iterate through values
	enum Joint_
	{
		HEAD,
		NECK,
		TORSO,
		LEFT_SHOULDER,
		LEFT_ELBOW,
		LEFT_HAND,
		RIGHT_SHOULDER,
		RIGHT_ELBOW,
		RIGHT_HAND,
		LEFT_HIP,
		LEFT_KNEE,
		LEFT_FOOT,
		RIGHT_HIP,
		RIGHT_KNEE,
		RIGHT_FOOT,
		NUM_JOINTS
	};
	typedef int Joint;
	
	enum Side_
	{
		LEFT,
		MIDDLE,
		RIGHT
	};
	typedef int Side;
	char const* sideName(Side side);
	bool isValidSide(Side side);
	
	char const* jointName(Joint jointId);
	char const* jointNameAbbr(Joint jointId);
	extern std::vector<std::string> const JOINT_NAMES;
	
	Side jointSide(Joint jointId);
	
	inline bool isValidJoint(Joint jointId)
	{
		return 0 <= jointId && jointId < NUM_JOINTS;
	}
	
	/// To assist with drawing we also define limbs: connections between
	/// joints
	enum Limb_
	{
		UPPER_SPINE, ///< Neck to head
		SPINE, ///< Torso to neck
		LEFT_COLLAR, ///< neck to shoulder
		RIGHT_COLLAR,
		LEFT_ABDOMEN, /// Torso to hip
		RIGHT_ABDOMEN,
		LEFT_FOREARM,
		LEFT_UPPERARM,
		RIGHT_FOREARM,
		RIGHT_UPPERARM,
		LEFT_THIGH,
		LEFT_CALF,
		RIGHT_THIGH,
		RIGHT_CALF,
		NUM_LIMBS
	};
	typedef int Limb;
	
	/// returns pair of joints. First element is parent of second
	/// from the torso outwards
	std::pair<Joint, Joint> limbJoints(Limb limb);
	
	char const* limbName(Limb limbId);
	char const* limbNameAbbr(Limb limbId);
	Side limbSide(Limb limbId);
	
	inline bool isValidLimb(Limb limbId)
	{
		return 0 <= limbId && limbId < NUM_LIMBS;
	}
	
	
	
	
	// --------------------------------------------------------------
	class Skeleton3d : public Base3dData
	{
	public:
		typedef Base3dData BaseType;
		
		explicit
		Skeleton3d(double timestamp = 0., int id = sDefaultId, SceneMetaPtr sceneMeta = SceneMeta::defaultSceneMeta());
		
//		std::vector<Point3d> const& joints() const { return mJoints; }
//		std::vector<Point3d>& joints() { return mJoints; }
		
		/// Joints are expressed in world space using right-handed coordinates.
		/// We follow the MS Kinect SDK standard:
		/// Origin is at the camera. From the camera's perspective, z extends
		/// out of the camera, x is horizontal going to the *left* and the
		/// y axis extends upwards. Measurements are in *metres*.
		Point3d& joint(Joint jointId);
		Point3d const& joint(Joint jointId) const;
		
		/// JointProjectives are expressed in the projected space of the
		/// camera. We follow the MS Kinect SDK standard:
		/// Origin is upper left pixel of the camera image. From the camera's
		/// perspective, z extends out of the camera, x is horizontal going
		/// to the *right* and the y axis extends upwards.
		/// NB This is a left-handed coordinate system - different from what
		/// we use for joints.
		Point3d& jointProjective(Joint jointId);
		Point3d const& jointProjective(Joint jointId) const;
		
		float& jointConfidence(Joint jointId);
		float const& jointConfidence(Joint jointId) const;
		
		virtual std::ostream& printTo(std::ostream& out) const override;
		virtual Type type() const override { return SKELETON3D; }
		virtual void draw() override;
		
		Skeleton3d operator+(Skeleton3d const& rhs) const;
		Skeleton3d operator-(Skeleton3d const& rhs) const;
		Skeleton3d operator*(Skeleton3d const& rhs) const;
		Skeleton3d operator/(Skeleton3d const& rhs) const;

		Skeleton3d& operator+=(Skeleton3d const& rhs);
		Skeleton3d& operator-=(Skeleton3d const& rhs);
		Skeleton3d& operator*=(Skeleton3d const& rhs);
		Skeleton3d& operator/=(Skeleton3d const& rhs);
		
		Skeleton3d operator+(Point3d const& rhs) const;
		Skeleton3d operator-(Point3d const& rhs) const;
		Skeleton3d operator*(Point3d const& rhs) const;
		Skeleton3d operator/(Point3d const& rhs) const;
		
		Skeleton3d& operator+=(Point3d const& rhs);
		Skeleton3d& operator-=(Point3d const& rhs);
		Skeleton3d& operator*=(Point3d const& rhs);
		Skeleton3d& operator/=(Point3d const& rhs);
		
//		Scene3d operator+(Scene3d const& rhs) const;
//		Scene3d operator-(Scene3d const& rhs) const;
//		Scene3d operator*(Scene3d const& rhs) const;
//		Scene3d operator/(Scene3d const& rhs) const;
		
		Skeleton3d operator*(Value const& rhs) const;
		Skeleton3d operator/(Value const& rhs) const;
		Skeleton3d& operator*=(Value const& rhs);
		Skeleton3d& operator/=(Value const& rhs);
		
		
		Skeleton3d operator*(double rhs) const;
		Skeleton3d operator/(double rhs) const;

		Skeleton3d& operator*=(double rhs);
		Skeleton3d& operator/=(double rhs);

		bool operator==(Skeleton3d const& rhs) const;
		bool operator!=(Skeleton3d const& rhs) const;
		
		Skeleton3d operator+() const { return *this; }
		Skeleton3d operator-() const;
		
	private:
		std::vector<Point3d> mJoints;
		std::vector<Point3d> mJointProjectives;
		std::vector<float> mJointConfidences;
	};
	
		
	inline
	Skeleton3d Skeleton3d::operator*(double rhs) const
	{
		Skeleton3d skel = *this;
		skel *= rhs;
		return skel;
	}
	
	
	inline
	Skeleton3d& Skeleton3d::operator*=(double rhs)
	{
		for (int i=0; i<NUM_JOINTS; i++)
		{
			joint(i) *= rhs;
			jointProjective(i) *= rhs;
			jointConfidence(i) *= rhs;
		}
		return *this;
	}

	inline
	Skeleton3d Skeleton3d::operator/(double rhs) const
	{
		Skeleton3d skel = *this;
		skel /= rhs;
		return skel;
	}
	
	
	inline
	Skeleton3d& Skeleton3d::operator/=(double rhs)
	{
		for (int i=0; i<NUM_JOINTS; i++)
		{
			joint(i) /= rhs;
			jointProjective(i) /= rhs;
			jointConfidence(i) /= rhs;
		}
		return *this;
	}
	
	inline
	Skeleton3d operator*(double lhs, Skeleton3d const& rhs)
	{
		return rhs * lhs;
	}

	inline
	Skeleton3d operator/(double lhs, Skeleton3d const& rhs)
	{
		return rhs / lhs;
	}

	
	inline Point3d& Skeleton3d::joint(Joint jointId)
	{
		assert(isValidJoint(jointId));
		return mJoints[jointId];
	}
	
	inline Point3d const& Skeleton3d::joint(Joint jointId) const
	{
		assert(isValidJoint(jointId));
		return mJoints[jointId];
	}
	
	inline Point3d& Skeleton3d::jointProjective(Joint jointId)
	{
		assert(isValidJoint(jointId));
		return mJointProjectives[jointId];
	}
	
	inline Point3d const& Skeleton3d::jointProjective(Joint jointId) const
	{
		assert(isValidJoint(jointId));
		return mJointProjectives[jointId];
	}
	
	inline float& Skeleton3d::jointConfidence(Joint jointId)
	{
		assert(isValidJoint(jointId));
		return mJointConfidences[jointId];
	}
	
	inline float const& Skeleton3d::jointConfidence(Joint jointId) const
	{
		assert(isValidJoint(jointId));
		return mJointConfidences[jointId];
	}
	
	inline
	Skeleton3d Skeleton3d::operator-() const
	{
		Skeleton3d out = *this;
		for (int i=0; i<NUM_JOINTS; i++)
		{
			out.joint(i) = -out.joint(i);
			out.jointProjective(i) = -out.jointProjective(i);
		}
		return out;
	}
	
#define hm_skeleton_define_free_operator(op, trait) \
	template <typename T> \
	typename trait<T, Skeleton3d>::result_type \
	operator op (T const& lhs, Skeleton3d const& rhs) \
	{ \
		Skeleton3d out(chooseTimestamp(lhs, rhs), chooseSceneMeta(lhs, rhs)); \
		for (int i=0; i<NUM_JOINTS; i++) \
		{ \
			out.joint(i) = lhs op rhs.joint(i); \
			out.jointProjective(i) = lhs op rhs.jointProjective(i); \
			out.jointConfidence(i) = rhs.jointConfidence(i); \
			out.id = chooseId(lhs, rhs); \
		} \
		return out; \
	}
	
	hm_skeleton_define_free_operator(+, supports_addition)
	hm_skeleton_define_free_operator(-, supports_addition)
	hm_skeleton_define_free_operator(*, supports_multiplication)
	hm_skeleton_define_free_operator(/, supports_multiplication)
}



