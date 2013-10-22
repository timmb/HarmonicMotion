#pragma once
#include "Common.h"
#include <cassert>
#include <vector>
#include "DataType.h"
#include "Point3d.h"

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
	
	const char* jointName(Joint jointId);
	const char* jointNameAbbr(Joint jointId);
	
	inline bool isValidJoint(Joint jointId)
	{
		return 0 <= jointId && jointId < NUM_JOINTS;
	}
	
	
	// --------------------------------------------------------------
	class Skeleton3d : public DataType
	{
	public:
		Skeleton3d();
		
		std::vector<Point3d> const& joints() const { return mJoints; }
		std::vector<Point3d>& joints() { return mJoints; }
		
		Point3d& joint(Joint jointId);
		Point3d const& joint(Joint jointId) const;
		
		Point3d& jointProjective(Joint jointId);
		Point3d const& jointProjective(Joint jointId) const;
		
		float& jointConfidence(Joint jointId);
		float const& jointConfidence(Joint jointId) const;
		
		int& id() { return mId; }
		int const& id() const { return mId; }
		
		virtual std::string toString() const override;
		virtual Type type() const override { return SKELETON3D; }
		
	private:
		int mId;
		std::vector<Point3d> mJoints;
		std::vector<Point3d> mJointProjectives;
		std::vector<float> mJointConfidences;
		
		friend std::ostream& operator<<(std::ostream&, Skeleton3d const&);
	};
	
	std::ostream& operator<<(std::ostream&, Skeleton3d const&);
	
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
}