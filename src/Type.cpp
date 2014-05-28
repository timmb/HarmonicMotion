#include "Type.h"
#include <cassert>


namespace std
{
	std::string to_string(hm::Type type)
	{
		switch (type)
		{
			case hm::UNDEFINED:
				return "Null";
			case hm::VALUE:
				return "Value";
			case hm::POINT3D:
				return "Point3d";
			case hm::SKELETON3D:
				return "Skeleton3d";
			case hm::SCENE3D:
				return "Scene3d";
			case hm::IMAGE3D:
				return "Image3d";
			case hm::ALL_TYPES:
				return "All types";
			case hm::VECTOR3D_TYPES:
				return "3D Vector types";
			default:
				assert(false && "Case missing for type value.");
		}
	}
}