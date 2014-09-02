//
//  Type.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once
#include <string>
#include <type_traits>
#include <vector>

namespace hm
{
	// Data types used in the pipeline
	enum Type
	{
		UNDEFINED = 0,
		VALUE = (1<<0), // float
		POINT2D = (1<<1),
		POINT3D = (1<<2), // Vector of three VALUEs
		SKELETON3D = (1<<3), // Set of POINT3Ds
		SCENE3D = (1<<4), // Set of SKELETON3Ds
		IMAGE2D = (1<<5),
		LIST_VALUE = (1<<6),
		LIST_POINT2D = (1<<7),
		LIST_POINT3D = (1<<8),
		
		ALL_TYPES = (1<<9) - 1,
		VECTOR3D_TYPES = POINT3D | SKELETON3D | SCENE3D | LIST_POINT3D,
		VECTOR2D_TYPES = POINT2D | LIST_POINT2D,
		SCALAR_TYPES = VALUE | LIST_VALUE,
		LIST_TYPES = LIST_VALUE | LIST_POINT2D | LIST_POINT3D,
		SCALABLE_TYPES = VECTOR3D_TYPES | VECTOR2D_TYPES | SCALAR_TYPES | IMAGE2D
	};
	static_assert((VALUE | POINT2D | POINT3D | SKELETON3D | SCENE3D | IMAGE2D | LIST_VALUE | LIST_POINT2D | LIST_POINT3D) == ALL_TYPES, "Types have not been properly defined");
	/// Can be an OR combination of types.
	typedef int Types;
	
	
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
	
	
	/// \return a list of all types, optionall including the UNDEFINED
	/// type (i.e. DataNull).
	std::vector<Type> const& listOfTypes(bool includeNull=false);
	
	
	/// You can use getType<T>() to return the type of T
	template <typename T>
	Type getType();
	
#define hm_define_get_type(T, typeEnumValue) \
template<> inline Type getType<T>() { return typeEnumValue; }
	
	hm_define_get_type(DataNull, UNDEFINED)
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

namespace std
{
	/// Get a string representation of a Type
	std::string to_string(hm::Type type);
}