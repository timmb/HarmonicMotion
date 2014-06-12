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
			case hm::POINT2D:
				return "Point2d";
			case hm::POINT3D:
				return "Point3d";
			case hm::SKELETON3D:
				return "Skeleton3d";
			case hm::SCENE3D:
				return "Scene3d";
			case hm::IMAGE2D:
				return "Image2d";
			case hm::LIST_VALUE:
				return "ListValue";
			case hm::LIST_POINT2D:
				return "ListPoint2d";
			case hm::LIST_POINT3D:
				return "ListPoint3d";
			case hm::ALL_TYPES:
				return "All types";
			case hm::VECTOR3D_TYPES:
				return "3D vector types";
			case hm::VECTOR2D_TYPES:
				return "2D vector types";
			case hm::SCALAR_TYPES:
				return "Scalar types";
			case hm::LIST_TYPES:
				return "List types";
			default:
				if (type<0 || type>hm::ALL_TYPES)
				{
					assert(false && "Case missing for type value.");
					return "Invalid type";
				}
				else
				{
					return "Multiple types";
				}
		}
	}
}