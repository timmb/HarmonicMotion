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
#include <type_traits>

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
	
	// forward declare data types
	class BaseData;
	class Base1dData;
	class Base2dData;
	class Base3dData;
	
	class DataNull;
	class Value;
	class Point2d;
	class Point3d;
	class Skeleton3d;
//	class Scene3d;
	class Image2d;
	template <typename DataType>
	class List;
	typedef List<Value> ListValue;
	typedef List<Point2d> ListPoint2d;
	typedef List<Point3d> ListPoint3d;
	typedef List<Skeleton3d> Scene3d;
	
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
	
	
	
	template <typename LHS, typename RHS>
	struct supports_addition : public std::false_type {};
	
	template <typename LHS, typename RHS>
	struct supports_multiplication : public std::false_type {};
//
//	// define a type trait indicating T and U can be added/subtracted
#define hm_enable_supports_addition(T, U) \
	template<> \
	struct supports_addition<T, U> : public std::true_type {};

	
#define hm_enable_supports_commutative_addition(T, U) \
	template<> \
	struct supports_addition<T, U> : public std::true_type {}; \
	template<> \
	struct supports_addition<U, T> : public std::true_type {};

	hm_enable_supports_addition(double, double)
	hm_enable_supports_addition(float, float)
	hm_enable_supports_addition(Value, Value)
	hm_enable_supports_addition(Point2d, Point2d)
	hm_enable_supports_addition(Image2d, Image2d)
	hm_enable_supports_addition(Point3d, Point3d)
	hm_enable_supports_addition(Skeleton3d, Skeleton3d)
	hm_enable_supports_addition(Scene3d, Scene3d)
	hm_enable_supports_addition(ListValue, ListValue)
	hm_enable_supports_addition(ListPoint2d, ListPoint2d)
	hm_enable_supports_addition(ListPoint3d, ListPoint3d)

	hm_enable_supports_commutative_addition(Value, Image2d)
	hm_enable_supports_commutative_addition(Value, ListValue)
	hm_enable_supports_commutative_addition(double, Image2d)
	hm_enable_supports_commutative_addition(double, ListValue)
	hm_enable_supports_commutative_addition(float, Image2d)
	hm_enable_supports_commutative_addition(float, ListValue)
	
	hm_enable_supports_commutative_addition(Point2d, ListPoint2d)
	hm_enable_supports_commutative_addition(Point3d, Skeleton3d)
	hm_enable_supports_commutative_addition(Point3d, Scene3d)
	hm_enable_supports_commutative_addition(Point3d, ListPoint3d)
	
	hm_enable_supports_commutative_addition(Skeleton3d, Scene3d)
	

	// define a type trait indicating T and U can be added/subtracted
	#define hm_enable_supports_multiplication(T, U) \
	template<> \
	struct supports_multiplication<T, U> \
	{ \
	static std::true_type value; \
	}; \

		
	#define hm_enable_supports_commutative_multiplication(T, U) \
	template<> \
	struct supports_multiplication<T, U> \
	{ \
	static std::true_type value; \
	}; \
	template<> \
	struct supports_multiplication<U, T> \
	{ \
	static std::true_type value; \
	};
	
	hm_enable_supports_multiplication(double, double)
	hm_enable_supports_multiplication(float, float)
	hm_enable_supports_multiplication(Value, Value)
	hm_enable_supports_multiplication(Point2d, Point2d)
	hm_enable_supports_multiplication(Image2d, Image2d)
	hm_enable_supports_multiplication(Point3d, Point3d)
	hm_enable_supports_multiplication(Skeleton3d, Skeleton3d)
	hm_enable_supports_multiplication(Scene3d, Scene3d)
	hm_enable_supports_multiplication(ListValue, ListValue)
	hm_enable_supports_multiplication(ListPoint2d, ListPoint2d)
	hm_enable_supports_multiplication(ListPoint3d, ListPoint3d)
	
	hm_enable_supports_commutative_multiplication(Value, Image2d)
	hm_enable_supports_commutative_multiplication(Value, ListValue)
	hm_enable_supports_commutative_multiplication(Value, Point2d)
	hm_enable_supports_commutative_multiplication(Value, ListPoint2d)
	hm_enable_supports_commutative_multiplication(Value, Point3d)
	hm_enable_supports_commutative_multiplication(Value, ListPoint3d)
	hm_enable_supports_commutative_multiplication(Value, Skeleton3d)
	hm_enable_supports_commutative_multiplication(Value, Scene3d)
	hm_enable_supports_commutative_multiplication(double, Image2d)
	hm_enable_supports_commutative_multiplication(double, ListValue)
	hm_enable_supports_commutative_multiplication(double, Point2d)
	hm_enable_supports_commutative_multiplication(double, ListPoint2d)
	hm_enable_supports_commutative_multiplication(double, Point3d)
	hm_enable_supports_commutative_multiplication(double, ListPoint3d)
	hm_enable_supports_commutative_multiplication(double, Skeleton3d)
	hm_enable_supports_commutative_multiplication(double, Scene3d)
	hm_enable_supports_commutative_multiplication(float, Image2d)
	hm_enable_supports_commutative_multiplication(float, ListValue)
	hm_enable_supports_commutative_multiplication(float, Point2d)
	hm_enable_supports_commutative_multiplication(float, ListPoint2d)
	hm_enable_supports_commutative_multiplication(float, Point3d)
	hm_enable_supports_commutative_multiplication(float, ListPoint3d)
	hm_enable_supports_commutative_multiplication(float, Skeleton3d)
	hm_enable_supports_commutative_multiplication(float, Scene3d)
	
	
	hm_enable_supports_commutative_multiplication(Point2d, ListPoint2d)
	hm_enable_supports_commutative_multiplication(Point3d, ListPoint3d)
	hm_enable_supports_commutative_multiplication(Point3d, Skeleton3d)
	hm_enable_supports_commutative_multiplication(Point3d, Scene3d)
	
	hm_enable_supports_commutative_multiplication(Skeleton3d, Scene3d)

	
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
