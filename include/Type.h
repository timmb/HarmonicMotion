//
//  Type.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once
#include <string>

namespace hm
{
	// Data types used in the pipeline
	enum Type
	{
		UNDEFINED = 0,
		VALUE = (1<<0), // float
		POINT3D = (1<<1), // Vector of three VALUEs
		SKELETON3D = (1<<2), // Set of POINT3Ds
		SCENE3D = (1<<3), // Set of SKELETON3Ds
		IMAGE2D = (1<<4),
		LIST_VALUE = (1<<5),
		LIST_POINT3D = (1<<6),
		ALL_TYPES = (1<<7) - 1,
		VECTOR3D_TYPES = POINT3D | SKELETON3D | SCENE3D,
		SCALABLE_TYPES = VALUE | VECTOR3D_TYPES | IMAGE2D,
		LIST_TYPES = LIST_VALUE | LIST_POINT3D
	};
	static_assert(VALUE | POINT3D | SKELETON3D | SCENE3D | IMAGE2D | LIST_VALUE | LIST_POINT3D == ALL_TYPES, "Types have not been properly defined");
	/// Can be an OR combination of types.
	typedef int Types;
}

namespace std
{
	std::string to_string(hm::Type type);
}