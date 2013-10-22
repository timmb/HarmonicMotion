//
//  Common.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 26/09/2013.
//
//

#pragma once
#include <memory>
#include <vector>

namespace hm
{
	// forward declarations
	class Inlet;
	class Outlet;
	class Node;
	// data types
	typedef float Value;
	typedef ci::Vec3f Point3d;
	class Skeleton3d;
	class Scene3d;
	
	// typedefs
	typedef std::shared_ptr<Inlet> InletPtr;
	typedef std::shared_ptr<Outlet> OutletPtr;
	typedef std::shared_ptr<Node> NodePtr;
	
	/// \return number of seconds since the program began
	double elapsedTime();

#if defined(DEBUG) || defined(_DEBUG)
//#define HM_LOG_DEBUG
#endif
	
#ifdef HM_LOG_DEBUG
#define __FILE_NO_DIR__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
	#define hm_debug(message) std::cerr << __FILE_NO_DIR__ << ' ' <<__LINE__<<' '<<message<<std::endl;
#else
#define hm_debug(message) ;
#endif
	
	// Data types used in the pipeline
	enum Type
	{
		UNDEFINED = 0,
		VALUE = (1<<0), // float
		POINT3D = (1<<1), // Vector of three VALUEs
		SKELETON3D = (1<<2), // Set of POINT3Ds
		SCENE3D = (1<<3), // Set of SKELETON3Ds
		IMAGE3D = (1<<4),
		ALL_TYPES = (1<<5) - 1
	};
	static_assert(VALUE | POINT3D | SKELETON3D | SCENE3D | IMAGE3D == ALL_TYPES, "Types have not been properly defined");
	/// Can be an OR combination of types.
	typedef int Types;
	
	std::string asString(Type type);
}