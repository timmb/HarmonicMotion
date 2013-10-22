#include "Skeleton3d.h"
#include <sstream>

namespace hm
{
	std::ostream& operator<<(std::ostream& out, Skeleton3d const& rhs)
	{
		out << "Skeleton3d id:"<<rhs.id()<<' ';
		for (int i=0; i<NUM_JOINTS; i++)
		{
			out << jointNameAbbr(i)<<':'
			<<rhs.joint(i)
			<<(rhs.jointConfidence(i)<0.9f?'+':'-')
			<<' ';
		}
		return out;
	}
}

using namespace hm;


const char* hm::jointName(Joint joint)
{
	assert(isValidJoint(joint));
	switch (joint)
	{
		case HEAD:
			return "Head";
		case LEFT_SHOULDER:
			return "Left shoulder";
		case LEFT_ELBOW:
			return "Left elbow";
		case LEFT_HAND:
			return "Left hand";
		case RIGHT_SHOULDER:
			return "Right shoulder";
		case RIGHT_ELBOW:
			return "Right elbow";
		case RIGHT_HAND:
			return "Right hand";
		case LEFT_HIP:
			return "Left hip";
		case LEFT_KNEE:
			return "Left knee";
		case LEFT_FOOT:
			return "Left foot";
		case RIGHT_HIP:
			return "Right hip";
		case RIGHT_KNEE:
			return "Right knee";
		case RIGHT_FOOT:
			return "Right foot";
		default:
			return "Unrecognised joint";
	}
}

const char* hm::jointNameAbbr(Joint joint)
{
	assert(isValidJoint(joint));
	switch (joint)
	{
		case HEAD:
			return "H";
		case LEFT_SHOULDER:
			return "LS";
		case LEFT_ELBOW:
			return "LE";
		case LEFT_HAND:
			return "LH";
		case RIGHT_SHOULDER:
			return "RS";
		case RIGHT_ELBOW:
			return "RE";
		case RIGHT_HAND:
			return "Rh";
		case LEFT_HIP:
			return "LH";
		case LEFT_KNEE:
			return "LK";
		case LEFT_FOOT:
			return "LF";
		case RIGHT_HIP:
			return "RH";
		case RIGHT_KNEE:
			return "RK";
		case RIGHT_FOOT:
			return "RF";
		default:
			return "?";
	}
}




Skeleton3d::Skeleton3d()
: mJoints(NUM_JOINTS)
{}


std::string Skeleton3d::toString() const
{
	return (std::stringstream() << *this).str();
}