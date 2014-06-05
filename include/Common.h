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
#include <ostream>
#include <iostream>
#include "Type.h"

namespace hm
{
	// forward declarations
	class Inlet;
	class Outlet;
	class Node;
	class BaseParameter;
	class PatchCord;
	class Pipeline;
	class Listener;
	
	// forward declare data types
	class DataNull;
	class Value;
	class Point3d;
	class Skeleton3d;
	class Scene3d;
	
	// typedefs
	typedef std::shared_ptr<Inlet> InletPtr;
	typedef std::shared_ptr<Outlet> OutletPtr;
	typedef std::shared_ptr<Node> NodePtr;
	typedef std::shared_ptr<BaseParameter> ParameterPtr;
	typedef std::shared_ptr<const BaseParameter> ParameterConstPtr;
	typedef std::shared_ptr<PatchCord> PatchCordPtr;
	typedef std::shared_ptr<Pipeline> PipelinePtr;
	
	/// \return number of seconds since the program began
	double elapsedTime();
	
	std::vector<Type> const& listOfTypes();
	
	// Check if STL container contains an element
	template <typename Container, typename Value>
	bool contains(Container container, Value const& value)
	{
		return std::find(container.begin(), container.end(), value) != container.end();
	}
	
	/// Add an indent to newlines on a string
	std::string indent(std::string const& s, int indentAmount=2);
	
	
	// For printing verbose debugging info
#if defined(DEBUG) || defined(_DEBUG)
	#ifndef HM_LOG_DEBUG
		#define HM_LOG_DEBUG
	#endif
#endif
	
#define HM__FILE_NO_DIR__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef HM_LOG_DEBUG
	#define hm_debug(message) std::cerr << HM__FILE_NO_DIR__ << ' ' <<__LINE__<<' '<<message<<std::endl;
#else
	#define hm_debug(message) ;;
#endif

#define hm_info(message) std::cerr << "* Info: "<<message<<std::endl;
#define hm_warning(message) std::cerr << "*Warning: "<<message<<std::endl;
	
#define hm_error(message) std::cerr << "* Error:" << HM__FILE_NO_DIR__ << ' ' << __LINE__<<'\n'<<message<<std::endl;
	

}