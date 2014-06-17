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
#include "TypeTraits.h"

namespace hm
{
	// forward declarations
	class Inlet;
	class Outlet;
	class Node;
	class BaseParameter;
	template <typename T>
	class Parameter;
	class PatchCord;
	class Pipeline;
	class Listener;
	class Renderer;
	
	
	// typedefs
	typedef std::shared_ptr<Inlet> InletPtr;
	typedef std::shared_ptr<Outlet> OutletPtr;
	typedef std::shared_ptr<Node> NodePtr;
	typedef std::shared_ptr<BaseParameter> ParameterPtr;
	template <typename T>
	using ParameterPtrT = std::shared_ptr<Parameter<T>>;
	typedef std::shared_ptr<const BaseParameter> ParameterConstPtr;
	typedef std::shared_ptr<PatchCord> PatchCordPtr;
	typedef std::shared_ptr<Pipeline> PipelinePtr;
	typedef std::shared_ptr<Listener> ListenerPtr;
	typedef std::shared_ptr<Renderer> RendererPtr;
	
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
	
	
	/// You can use getType<T>() to return the type of T
	template <typename T>
	Type getType();
	
#define hm_define_get_type(T, typeEnumValue) \
	template<> Type getType<T>() { return typeEnumValue; }
	
	hm_define_get_type(Value, VALUE)
	hm_define_get_type(Point2d, POINT2D)
	hm_define_get_type(Point3d, POINT3D)
	hm_define_get_type(Skeleton3d, SKELETON3D)
	hm_define_get_type(Scene3d, SCENE3D)
	hm_define_get_type(Image2d, IMAGE2D)
	hm_define_get_type(List<Value>, LIST_VALUE)
	hm_define_get_type(List<Point2d>, LIST_POINT2D)
	hm_define_get_type(List<Point3d>, LIST_POINT3D)
	
	// Get a string representation of a Data type
	template<typename T>
	std::string stringRepresentation()
	{
		static_assert(std::is_base_of<BaseData, T>::value, "stringRepresentation() only defined for derivatives of BaseData");
		return std::to_string(getType<T>());
	}
	
	
	
}

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
