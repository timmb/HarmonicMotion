#include "Skeleton3d.h"
#include "Point3d.h"
#include "Value.h"
#include "Scene3d.h"
#include "cinder/gl/gl.h"
#include <vector>

using namespace std;

namespace hm
{

	std::ostream& Skeleton3d::printTo(std::ostream& out) const
	{
		out << "Skeleton3d id:" << id << ' ';
		for (int i = 0; i < NUM_JOINTS; i++)
		{
			out << jointNameAbbr(i) << ':'
				<< joint(i) << " (Proj: "<<jointProjective(i).value<<")"
				<< (jointConfidence(i) < 0.9f ? '+' : '-')
				<< ' ';
		}
		return out;
	}

	char const* sideName(Side side)
	{
		switch (side)
		{
		case LEFT:
			return "left";
		case MIDDLE:
			return "middle";
		case RIGHT:
			return "right";
		default:
			return "invalid side value";
		}
	}

	bool isValidSide(Side side)
	{
		return side == LEFT || side == MIDDLE || side == RIGHT;
	}

	Side jointSide(Joint joint)
	{
		assert(isValidJoint(joint));
		switch (joint)
		{
		case HEAD:
		case NECK:
		case SPINE_TOP:
		case SPINE_MID:
		case SPINE_BASE:
			return MIDDLE;
		case LEFT_SHOULDER:
		case LEFT_ELBOW:
		case LEFT_HAND:
		case LEFT_WRIST:
		case LEFT_HAND_TIP:
		case LEFT_THUMB:
			return LEFT;
		case RIGHT_SHOULDER:
		case RIGHT_ELBOW:
		case RIGHT_HAND:
		case RIGHT_HAND_TIP:
		case RIGHT_THUMB:
			return RIGHT;
		case LEFT_HIP:
		case LEFT_KNEE:
		case LEFT_ANKLE:
		case LEFT_FOOT:
			return LEFT;
		case RIGHT_HIP:
		case RIGHT_KNEE:
		case RIGHT_ANKLE:
		case RIGHT_FOOT:
			return RIGHT;
		default:
			assert(false);
			return MIDDLE;
		}
	}


	const char* jointName(Joint joint)
	{
		assert(isValidJoint(joint));
		switch (joint)
		{
		case HEAD:
			return "Head";
		case NECK:
			return "Neck";
		case SPINE_TOP:
			return "Spine top";
		case SPINE_MID:
			return "Spine middle";
		case SPINE_BASE:
			return "Spine base";
		case LEFT_SHOULDER:
			return "Left shoulder";
		case LEFT_ELBOW:
			return "Left elbow";
		case LEFT_WRIST:
			return "Left wrist";
		case LEFT_HAND:
			return "Left hand";
		case LEFT_HAND_TIP:
			return "Left hand tip";
		case LEFT_THUMB:
			return "Left thumb";
		case RIGHT_SHOULDER:
			return "Right shoulder";
		case RIGHT_ELBOW:
			return "Right elbow";
		case RIGHT_WRIST:
			return "Right wrist";
		case RIGHT_HAND:
			return "Right hand";
		case RIGHT_HAND_TIP:
			return "Right hand tip";
		case RIGHT_THUMB:
			return "Right thumb";
		case LEFT_HIP:
			return "Left hip";
		case LEFT_KNEE:
			return "Left knee";
		case LEFT_ANKLE:
			return "Left ankle";
		case LEFT_FOOT:
			return "Left foot";
		case RIGHT_HIP:
			return "Right hip";
		case RIGHT_KNEE:
			return "Right knee";
		case RIGHT_ANKLE:
			return "Right ankle";
		case RIGHT_FOOT:
			return "Right foot";
		default:
			assert(false);
			return "Unrecognised joint";
		}
	}

	const char* jointNameAbbr(Joint joint)
	{
		assert(isValidJoint(joint));
		switch (joint)
		{
		case HEAD:
			return "H";
		case NECK:
			return "N";
		case SPINE_TOP:
			return "ST";
		case SPINE_MID:
			return "SM";
		case SPINE_BASE:
			return "SB";
		case LEFT_SHOULDER:
			return "LS";
		case LEFT_ELBOW:
			return "LE";
		case LEFT_WRIST:
			return "LW";
		case LEFT_HAND:
			return "LH";
		case LEFT_HAND_TIP:
			return "LHT";
		case LEFT_THUMB:
			return "LT";
		case RIGHT_SHOULDER:
			return "RS";
		case RIGHT_ELBOW:
			return "RE";
		case RIGHT_WRIST:
			return "RW";
		case RIGHT_HAND:
			return "RH";
		case RIGHT_HAND_TIP:
			return "RHT";
		case RIGHT_THUMB:
			return "RT";
		case LEFT_HIP:
			return "LH";
		case LEFT_KNEE:
			return "LK";
		case LEFT_ANKLE:
			return "LA";
		case LEFT_FOOT:
			return "LF";
		case RIGHT_HIP:
			return "RH";
		case RIGHT_KNEE:
			return "RK";
		case RIGHT_ANKLE:
			return "RA";
		case RIGHT_FOOT:
			return "RF";
		default:
			return "?";
		}
	}

	std::vector<std::string> const JOINT_NAMES = ([]() {
		std::vector<std::string> v;
		for (int i = 0; i < NUM_JOINTS; i++)
		{
			v.push_back(jointName(i));
		}
		return v;
	})();


	std::pair<Joint, Joint> limbJoints(Limb limb)
	{
		assert(isValidLimb(limb));
		switch (limb)
		{
		case NECK_TO_HEAD:
			return std::pair<Joint, Joint>(NECK, HEAD);
		case SPINE_TO_NECK:
			return std::pair<Joint, Joint>(SPINE_TOP, NECK);
		case UPPER_SPINE:
			return std::pair<Joint, Joint>(SPINE_MID, SPINE_TOP);
		case LOWER_SPINE:
			return std::pair<Joint, Joint>(SPINE_BASE, SPINE_MID);
		case LEFT_COLLAR:
			return std::pair<Joint, Joint>(SPINE_TOP, LEFT_SHOULDER);
		case RIGHT_COLLAR:
			return std::pair<Joint, Joint>(SPINE_TOP, RIGHT_SHOULDER);
		case LEFT_ABDOMEN:
			return std::pair<Joint, Joint>(SPINE_BASE, LEFT_HIP);
		case RIGHT_ABDOMEN:
			return std::pair<Joint, Joint>(SPINE_BASE, RIGHT_HIP);
		case LEFT_FOREARM:
			return std::pair<Joint, Joint>(LEFT_SHOULDER, LEFT_ELBOW);
		case LEFT_UPPERARM:
			return std::pair<Joint, Joint>(LEFT_ELBOW, LEFT_WRIST);
		case LEFT_WRIST_TO_HAND:
			return std::pair<Joint, Joint>(LEFT_WRIST, LEFT_HAND);
		case LEFT_HAND_TO_THUMB:
			return std::pair<Joint, Joint>(LEFT_HAND, LEFT_THUMB);
		case LEFT_FINGERS:
			return std::pair<Joint, Joint>(LEFT_HAND, LEFT_HAND_TIP);
		case RIGHT_FOREARM:
			return std::pair<Joint, Joint>(RIGHT_SHOULDER, RIGHT_ELBOW);
		case RIGHT_UPPERARM:
			return std::pair<Joint, Joint>(RIGHT_ELBOW, RIGHT_WRIST);
		case RIGHT_WRIST_TO_HAND:
			return std::pair<Joint, Joint>(RIGHT_WRIST, RIGHT_HAND);
		case RIGHT_HAND_TO_THUMB:
			return std::pair<Joint, Joint>(RIGHT_HAND, RIGHT_THUMB);
		case RIGHT_FINGERS:
			return std::pair<Joint, Joint>(RIGHT_HAND, RIGHT_HAND_TIP);
		case LEFT_THIGH:
			return std::pair<Joint, Joint>(LEFT_HIP, LEFT_KNEE);
		case LEFT_CALF:
			return std::pair<Joint, Joint>(LEFT_KNEE, LEFT_ANKLE);
		case LEFT_ANKLE_TO_FOOT:
			return std::pair<Joint, Joint>(LEFT_ANKLE, LEFT_FOOT);
		case RIGHT_THIGH:
			return std::pair<Joint, Joint>(RIGHT_HIP, RIGHT_KNEE);
		case RIGHT_CALF:
			return std::pair<Joint, Joint>(RIGHT_KNEE, RIGHT_ANKLE);
		case RIGHT_ANKLE_TO_FOOT:
			return std::pair<Joint, Joint>(RIGHT_ANKLE, RIGHT_FOOT);
		default:
			assert(false);
			return std::pair<Joint, Joint>(-1, -1);
		}
	}

	const char* limbName(Limb limbId)
	{
		assert(isValidLimb(limbId));
		switch (limbId)
		{
		case NECK_TO_HEAD:
			return "Neck to head";
		case SPINE_TO_NECK:
			return "Spint to neck";
		case UPPER_SPINE:
			return "Upper spine";
		case LOWER_SPINE:
			return "Lower spine";
		case LEFT_COLLAR:
			return "Left collar";
		case RIGHT_COLLAR:
			return "Right collar";
		case LEFT_ABDOMEN:
			return "Left abdomen";
		case RIGHT_ABDOMEN:
			return "Right abdomen";
		case LEFT_FOREARM:
			return "Left forearm";
		case LEFT_UPPERARM:
			return "Left upperarm";
		case LEFT_WRIST_TO_HAND:
			return "Left wrist to hand";
		case LEFT_HAND_TO_THUMB:
			return "Left hand to thumb";
		case LEFT_FINGERS:
			return "Left fingers";
		case RIGHT_FOREARM:
			return "Right forearm";
		case RIGHT_UPPERARM:
			return "Right upperarm";
		case RIGHT_WRIST_TO_HAND:
			return "Right wrist to hand";
		case RIGHT_HAND_TO_THUMB:
			return "Right hand to thumb";
		case RIGHT_FINGERS:
			return "Right fingers";
		case LEFT_THIGH:
			return "Left thigh";
		case LEFT_CALF:
			return "Left calf";
		case LEFT_ANKLE_TO_FOOT:
			return "Left ankle to foot";
		case RIGHT_THIGH:
			return "Right thigh";
		case RIGHT_CALF:
			return "Right calf";
		case RIGHT_ANKLE_TO_FOOT:
			return "Right ankle to foot";
		default:
			assert(false);
			return "(Unrecognised limb)";
		}
	}


	Side limbSide(Limb limbId)
	{
		assert(isValidLimb(limbId));
		switch (limbId)
		{
		case NECK_TO_HEAD:
		case SPINE_TO_NECK:
		case UPPER_SPINE:
		case LOWER_SPINE:
			return MIDDLE;
		case LEFT_COLLAR:
		case LEFT_FOREARM:
		case LEFT_UPPERARM:
		case LEFT_WRIST_TO_HAND:
		case LEFT_HAND_TO_THUMB:
		case LEFT_FINGERS:
			return LEFT;
		case RIGHT_COLLAR:
		case RIGHT_UPPERARM:
		case RIGHT_FOREARM:
		case RIGHT_WRIST_TO_HAND:
		case RIGHT_HAND_TO_THUMB:
		case RIGHT_FINGERS:
			return RIGHT;
		case LEFT_ABDOMEN:
		case LEFT_THIGH:
		case LEFT_CALF:
		case LEFT_ANKLE_TO_FOOT:
			return LEFT;
		case RIGHT_ABDOMEN:
		case RIGHT_THIGH:
		case RIGHT_CALF:
		case RIGHT_ANKLE_TO_FOOT:
			return RIGHT;
		default:
			assert(false);
			return MIDDLE;
		}
	}

	const char* limbNameAbbr(Limb limbId)
	{
		assert(isValidLimb(limbId));
		switch (limbId)
		{
		case NECK_TO_HEAD:
			return "NeHe";
		case SPINE_TO_NECK:
			return "SpNe";
		case UPPER_SPINE:
			return "USp";
		case LOWER_SPINE:
			return "LSp";
		case LEFT_COLLAR:
			return "LCo";
		case RIGHT_COLLAR:
			return "RCo";
		case LEFT_ABDOMEN:
			return "LAb";
		case RIGHT_ABDOMEN:
			return "RAb";
		case LEFT_FOREARM:
			return "LFa";
		case LEFT_UPPERARM:
			return "LUa";
		case RIGHT_FOREARM:
			return "RFa";
		case RIGHT_UPPERARM:
			return "RUa";
		case LEFT_WRIST_TO_HAND:
			return "LWrHa";
		case RIGHT_WRIST_TO_HAND:
			return "RWrHa";
		case LEFT_HAND_TO_THUMB:
			return "LHaTh";
		case RIGHT_HAND_TO_THUMB:
			return "RHaTh";
		case LEFT_THIGH:
			return "LTh";
		case LEFT_CALF:
			return "LCa";
		case LEFT_ANKLE_TO_FOOT:
			return "LAnFo";
		case RIGHT_THIGH:
			return "RTh";
		case RIGHT_CALF:
			return "RCa";
		case RIGHT_ANKLE_TO_FOOT:
			return "RAnFo";
		default:
			assert(false);
			return "?";
		}
	}
}

using namespace hm;


Skeleton3d::Skeleton3d(double timestamp, int id, SceneMetaPtr sceneMeta)
: Base3dData(timestamp, id, sceneMeta)
, mJoints(NUM_JOINTS)
, mJointProjectives(NUM_JOINTS)
, mJointConfidences(NUM_JOINTS)
{
	assert(sceneMeta != nullptr);
}



void Skeleton3d::draw()
{
	ci::gl::lineWidth(2.f);
	ci::gl::color(255, 255, 255);
	for (Limb l = 0; l < NUM_LIMBS; l++)
	{
		pair<Joint, Joint> joints = limbJoints(l);
		ci::gl::drawLine(joint(joints.first).value, joint(joints.second).value);
	}
}


// MARK: Operators


Skeleton3d Skeleton3d::operator+(Skeleton3d const& rhs) const
{
	Skeleton3d out(chooseTimestamp(*this, rhs), chooseId(*this, rhs), chooseSceneMeta(*this, rhs));
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		out.joint(i) = joint(i) + rhs.joint(i);
		out.jointProjective(i) = jointProjective(i) + rhs.jointProjective(i);
		out.jointConfidence(i) = jointConfidence(i) + rhs.jointConfidence(i);
	}
	return out;
}

Skeleton3d Skeleton3d::operator-(Skeleton3d const& rhs) const
{
	Skeleton3d out(chooseTimestamp(*this, rhs), chooseId(*this, rhs), chooseSceneMeta(*this, rhs));
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		out.joint(i) = joint(i) - rhs.joint(i);
		out.jointProjective(i) = jointProjective(i) - rhs.jointProjective(i);
		out.jointConfidence(i) = jointConfidence(i) - rhs.jointConfidence(i);
	}
	return out;
}

Skeleton3d Skeleton3d::operator*(Skeleton3d const& rhs) const
{
	Skeleton3d out(chooseTimestamp(*this, rhs), chooseId(*this, rhs), chooseSceneMeta(*this, rhs));
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		out.joint(i) = joint(i) * rhs.joint(i);
		out.jointProjective(i) = jointProjective(i) * rhs.jointProjective(i);
		out.jointConfidence(i) = jointConfidence(i) * rhs.jointConfidence(i);
	}
	return out;
}

Skeleton3d Skeleton3d::operator/(Skeleton3d const& rhs) const
{
	Skeleton3d out(chooseTimestamp(*this, rhs), chooseId(*this, rhs), chooseSceneMeta(*this, rhs));
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		out.joint(i) = joint(i) / rhs.joint(i);
		out.jointProjective(i) = jointProjective(i) / rhs.jointProjective(i);
		out.jointConfidence(i) = jointConfidence(i) / rhs.jointConfidence(i);
	}
	return out;
}


Skeleton3d& Skeleton3d::operator+=(Skeleton3d const& rhs)
{
	timestamp = chooseTimestamp(*this, rhs);
	id = chooseId(*this, rhs);
	sceneMeta = chooseSceneMeta(*this, rhs);

	for (int i = 0; i < NUM_JOINTS; i++)
	{
		joint(i) += rhs.joint(i);
		jointProjective(i) += rhs.jointProjective(i);
		jointConfidence(i) += rhs.jointConfidence(i);
	}
	return *this;
}

Skeleton3d& Skeleton3d::operator-=(Skeleton3d const& rhs)
{
	timestamp = chooseTimestamp(*this, rhs);
	id = chooseId(*this, rhs);
	sceneMeta = chooseSceneMeta(*this, rhs);

	for (int i = 0; i < NUM_JOINTS; i++)
	{
		joint(i) -= rhs.joint(i);
		jointProjective(i) -= rhs.jointProjective(i);
		jointConfidence(i) -= rhs.jointConfidence(i);
	}
	return *this;
}

Skeleton3d& Skeleton3d::operator*=(Skeleton3d const& rhs)
{
	timestamp = chooseTimestamp(*this, rhs);
	id = chooseId(*this, rhs);
	sceneMeta = chooseSceneMeta(*this, rhs);

	for (int i = 0; i < NUM_JOINTS; i++)
	{
		joint(i) *= rhs.joint(i);
		jointProjective(i) *= rhs.jointProjective(i);
		jointConfidence(i) *= rhs.jointConfidence(i);
	}
	return *this;
}

Skeleton3d& Skeleton3d::operator/=(Skeleton3d const& rhs)
{
	timestamp = chooseTimestamp(*this, rhs);
	id = chooseId(*this, rhs);
	sceneMeta = chooseSceneMeta(*this, rhs);

	for (int i = 0; i < NUM_JOINTS; i++)
	{
		joint(i) /= rhs.joint(i);
		jointProjective(i) /= rhs.jointProjective(i);
		jointConfidence(i) /= rhs.jointConfidence(i);
	}
	return *this;
}




Skeleton3d Skeleton3d::operator+(Point3d const& rhs) const
{
	Skeleton3d out(chooseTimestamp(*this, rhs), chooseId(*this, rhs), chooseSceneMeta(*this, rhs));
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		out.joint(i) = joint(i) + rhs;
		out.jointProjective(i) = jointProjective(i) + rhs;
		out.jointConfidence(i) = jointConfidence(i);
	}
	return out;
}

Skeleton3d Skeleton3d::operator-(Point3d const& rhs) const
{
	Skeleton3d out(chooseTimestamp(*this, rhs), chooseId(*this, rhs), chooseSceneMeta(*this, rhs));
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		out.joint(i) = joint(i) - rhs;
		out.jointProjective(i) = jointProjective(i) - rhs;
		out.jointConfidence(i) = jointConfidence(i);
	}
	return out;
}

Skeleton3d Skeleton3d::operator*(Point3d const& rhs) const
{
	Skeleton3d out(chooseTimestamp(*this, rhs), chooseId(*this, rhs), chooseSceneMeta(*this, rhs));
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		out.joint(i) = joint(i) * rhs;
		out.jointProjective(i) = jointProjective(i) * rhs;
		out.jointConfidence(i) = jointConfidence(i);
	}
	return out;
}

Skeleton3d Skeleton3d::operator/(Point3d const& rhs) const
{
	Skeleton3d out(chooseTimestamp(*this, rhs), chooseId(*this, rhs), chooseSceneMeta(*this, rhs));
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		out.joint(i) = joint(i) / rhs;
		out.jointProjective(i) = jointProjective(i) / rhs;
		out.jointConfidence(i) = jointConfidence(i);
	}
	return out;
}



Skeleton3d& Skeleton3d::operator+=(Point3d const& rhs)
{
	timestamp = chooseTimestamp(*this, rhs);
	id = chooseId(*this, rhs);
	sceneMeta = chooseSceneMeta(*this, rhs);
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		joint(i) += rhs;
		jointProjective(i) += rhs;
	}
	return *this;
}

Skeleton3d& Skeleton3d::operator-=(Point3d const& rhs)
{
	timestamp = chooseTimestamp(*this, rhs);
	id = chooseId(*this, rhs);
	sceneMeta = chooseSceneMeta(*this, rhs);
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		joint(i) -= rhs;
		jointProjective(i) -= rhs;
	}
	return *this;
}

Skeleton3d& Skeleton3d::operator*=(Point3d const& rhs)
{
	timestamp = chooseTimestamp(*this, rhs);
	id = chooseId(*this, rhs);
	sceneMeta = chooseSceneMeta(*this, rhs);
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		joint(i) *= rhs;
		jointProjective(i) *= rhs;
	}
	return *this;
}

Skeleton3d& Skeleton3d::operator/=(Point3d const& rhs)
{
	timestamp = chooseTimestamp(*this, rhs);
	id = chooseId(*this, rhs);
	sceneMeta = chooseSceneMeta(*this, rhs);
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		joint(i) /= rhs;
		jointProjective(i) /= rhs;
	}
	return *this;
}


//Scene3d Skeleton3d::operator+(Scene3d const& rhs) const
//{
//	return rhs + *this;
//}
//
//Scene3d Skeleton3d::operator-(Scene3d const& rhs) const
//{
//	return rhs - *this;
//}
//
//Scene3d Skeleton3d::operator*(Scene3d const& rhs) const
//{
//	return rhs * *this;
//}
//
//Scene3d Skeleton3d::operator/(Scene3d const& rhs) const
//{
//	return rhs / *this;
//}


Skeleton3d& Skeleton3d::operator*=(Value const& rhs)
{
	timestamp = chooseTimestamp(*this, rhs);
	id = chooseId(*this, rhs);
	sceneMeta = chooseSceneMeta(*this, rhs);
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		joint(i) *= rhs;
		jointProjective(i) *= rhs;
	}
	return *this;
}

Skeleton3d& Skeleton3d::operator/=(Value const& rhs)
{
	timestamp = chooseTimestamp(*this, rhs);
	id = chooseId(*this, rhs);
	sceneMeta = chooseSceneMeta(*this, rhs);
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		joint(i) /= rhs;
		jointProjective(i) /= rhs;
	}
	return *this;
}


Skeleton3d Skeleton3d::operator*(Value const& rhs) const
{
	Skeleton3d out(chooseTimestamp(*this, rhs), chooseId(*this, rhs), chooseSceneMeta(*this, rhs));
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		out.joint(i) = joint(i) * rhs;
		out.jointProjective(i) = jointProjective(i) * rhs;
		out.jointConfidence(i) = jointConfidence(i);
	}
	return out;
}

Skeleton3d Skeleton3d::operator/(Value const& rhs) const
{
	Skeleton3d out(chooseTimestamp(*this, rhs), chooseId(*this, rhs), chooseSceneMeta(*this, rhs));
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		out.joint(i) = joint(i) / rhs;
		out.jointProjective(i) = jointProjective(i) / rhs;
		out.jointConfidence(i) = jointConfidence(i);
	}
	return out;
}



//Skeleton3d& Skeleton3d::operator+=(Skeleton3d const& rhs)
//{
//	assert(sceneMeta == rhs.sceneMeta);
//	for (int i=0; i<NUM_JOINTS; i++)
//	{
//		joint(i) += rhs.joint(i);
//		jointProjective(i) += rhs.jointProjective(i);
//		jointConfidence(i) += rhs.jointConfidence(i);
//	}
//	return *this;
//}
//
//Skeleton3d& Skeleton3d::operator-=(Skeleton3d const& rhs)
//{
//	assert(sceneMeta == rhs.sceneMeta);
//	for (int i=0; i<NUM_JOINTS; i++)
//	{
//		joint(i) -= rhs.joint(i);
//		jointProjective(i) -= rhs.jointProjective(i);
//		jointConfidence(i) -= rhs.jointConfidence(i);
//	}
//	return *this;
//}



bool Skeleton3d::operator==(Skeleton3d const& rhs) const
{
	if (sceneMeta != rhs.sceneMeta)
		return false;
	for (int i = 0; i < NUM_JOINTS; i++)
	{
		if (joint(i) != rhs.joint(i))
			return false;
		if (jointProjective(i) != rhs.jointProjective(i))
			return false;
		if (jointConfidence(i) != rhs.jointConfidence(i))
			return false;
	}
	return true;
}

bool Skeleton3d::operator!=(Skeleton3d const& rhs) const
{
	return !(*this == rhs);
}





