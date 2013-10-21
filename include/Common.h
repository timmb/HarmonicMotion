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
	
	// typedefs
	typedef std::shared_ptr<Inlet> InletPtr;
	typedef std::shared_ptr<Outlet> OutletPtr;
	typedef std::shared_ptr<Node> NodePtr;
	
	/// \return number of seconds since the program began
	double elapsedTime();

#if defined(DEBUG) || defined(_DEBUG)
#define HM_LOG_DEBUG
#endif
	
#ifdef HM_LOG_DEBUG
	#define hm_debug(message) std::cerr << __FILE__ << ' ' <<__LINE__<<' '<<message<<std::endl;
#else
#define hm_debug(message)
#endif
	
	// Data types used in the pipeline
	enum Type
	{
		UNDEFINED,
		VALUE, // float
		POINT3D, // Vector of three VALUEs
		SKELETON3D, // Set of POINT3Ds
		SCENE3D, // Set of SKELETON3Ds
		IMAGE3D
	};
	
	std::string asString(Type type);
}