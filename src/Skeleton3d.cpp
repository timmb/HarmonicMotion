#include "Skeleton3d.h"
#include "Point3d.h"
#include "cinder/gl/gl.h"

using namespace hm;
using namespace std;


std::ostream& Skeleton3d::printTo(std::ostream& out) const
{
	out << "Skeleton3d id:"<<id()<<' ';
	for (int i=0; i<NUM_JOINTS; i++)
	{
		out << jointNameAbbr(i)<<':'
		<<joint(i)
		<<(jointConfidence(i)<0.9f?'+':'-')
		<<' ';
	}
	return out;
}


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


std::pair<Joint, Joint> hm::limbJoints(Limb limb)
{
	assert(isValidLimb(limb));
	switch (limb)
	{
		case UPPER_SPINE:
			return std::pair<Joint, Joint>(NECK,HEAD);
		case SPINE:
			return std::pair<Joint, Joint>(TORSO,NECK);
		case LEFT_COLLAR:
			return std::pair<Joint, Joint>(NECK,LEFT_SHOULDER);
		case RIGHT_COLLAR:
			return std::pair<Joint, Joint>(NECK,RIGHT_SHOULDER);
		case LEFT_ABDOMEN:
			return std::pair<Joint, Joint>(TORSO,LEFT_HIP);
		case RIGHT_ABDOMEN:
			return std::pair<Joint, Joint>(TORSO,RIGHT_HIP);
		case LEFT_FOREARM:
			return std::pair<Joint, Joint>(LEFT_SHOULDER,LEFT_ELBOW);
		case LEFT_UPPERARM:
			return std::pair<Joint, Joint>(LEFT_ELBOW,LEFT_HAND);
		case RIGHT_FOREARM:
			return std::pair<Joint, Joint>(RIGHT_SHOULDER,RIGHT_ELBOW);
		case RIGHT_UPPERARM:
			return std::pair<Joint, Joint>(RIGHT_ELBOW,RIGHT_HAND);
		case LEFT_THIGH:
			return std::pair<Joint, Joint>(LEFT_HIP,LEFT_KNEE);
		case LEFT_CALF:
			return std::pair<Joint, Joint>(LEFT_KNEE,LEFT_FOOT);
		case RIGHT_THIGH:
			return std::pair<Joint, Joint>(RIGHT_HIP,RIGHT_KNEE);
		case RIGHT_CALF:
			return std::pair<Joint, Joint>(RIGHT_KNEE,RIGHT_FOOT);
		default:
			assert(false);
			return std::pair<Joint,Joint>(-1, -1);
	}
}

const char* hm::limbName(Limb limbId)
{
	assert(isValidLimb(limbId));
	switch (limbId)
	{
		case UPPER_SPINE:
			return "Upper spine";
		case SPINE:
			return "Spine";
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
		case RIGHT_FOREARM:
			return "Right forearm";
		case RIGHT_UPPERARM:
			return "Right upperarm";
		case LEFT_THIGH:
			return "Left thigh";
		case LEFT_CALF:
			return "Left calf";
		case RIGHT_THIGH:
			return "Right thigh";
		case RIGHT_CALF:
			return "Right calf";
		default:
			assert(false);
			return "(Unrecognised limb)";
	}
}

const char* limbNameAbbr(Limb limbId)
{
	assert(isValidLimb(limbId));
	switch (limbId)
	{
		case UPPER_SPINE:
			return "USp";
		case SPINE:
			return "Sp";
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
		case LEFT_THIGH:
			return "LTh";
		case LEFT_CALF:
			return "LCa";
		case RIGHT_THIGH:
			return "RTh";
		case RIGHT_CALF:
			return "RCa";
		default:
			assert(false);
			return "?";
	}
}




Skeleton3d::Skeleton3d()
: mJoints(NUM_JOINTS)
, mJointProjectives(NUM_JOINTS)
, mJointConfidences(NUM_JOINTS)
{}



void Skeleton3d::draw()
{
	ci::gl::lineWidth(2.f);
	ci::gl::color(255, 255, 255);
	for (Limb l=0; l<NUM_LIMBS; l++)
	{
		pair<Joint,Joint> joints = limbJoints(l);
		ci::gl::drawLine(joint(joints.first), joint(joints.second));
	}
}

