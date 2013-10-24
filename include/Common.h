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
#include "Type.h"

namespace hm
{
	// forward declarations
	class Inlet;
	class Outlet;
	class Node;
	
	// forward declare data types
	class Value;
	class Point3d;
	class Skeleton3d;
	class Scene3d;
	
	// typedefs
	typedef std::shared_ptr<Inlet> InletPtr;
	typedef std::shared_ptr<Outlet> OutletPtr;
	typedef std::shared_ptr<Node> NodePtr;
	
	/// \return number of seconds since the program began
	double elapsedTime();
	
	std::vector<Type> const& listOfTypes();

	
	// For printing verbose debugging info
#if defined(DEBUG) || defined(_DEBUG)
//#define HM_LOG_DEBUG
#endif
	
#ifdef HM_LOG_DEBUG
	#define __FILE_NO_DIR__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
	#define hm_debug(message) std::cerr << __FILE_NO_DIR__ << ' ' <<__LINE__<<' '<<message<<std::endl;
#else
	#define hm_debug(message) ;;
#endif
	

}