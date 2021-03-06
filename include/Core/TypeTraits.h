#pragma once
#include <type_traits>
#include "Type.h"

namespace hm
{
	/// Has member value
	template <typename T>
	struct additive_identity {};
	
	template <typename T>
	struct multiplicative_identity {};
	
	// define a type trait indicating T and U can be added/subtracted
	// with return type R
	template <typename LHS, typename RHS>
	struct supports_addition : public std::false_type {
		typedef additive_identity<LHS> lhs_identity;
	};
	
	template <typename LHS, typename RHS>
	struct supports_multiplication : public std::false_type {};
	
	template <typename T>
	struct is_list : public std::false_type {};
	
	template <typename T>
	struct is_list<List<T>> : public std::true_type {};
	
	/// Trait to get a type from a "Type" enumeration value at compile time
	template <Type E>
	struct type_from_enum {};
	
	/// Trait determining if a value supports maximum(T,T)
	template <typename T>
	struct supports_maximum : public std::false_type {};
	
	template <typename T>
	struct supports_minimum : public std::false_type {};
	
	
#define hm_declare_additive_identity(Type, identity, IdentityType) \
template <> \
struct additive_identity<Type> { \
static IdentityType const value; \
};
	
#define hm_declare_multiplicative_identity(Type, identity, IdentityType) \
template <> \
struct multiplicative_identity<Type> { \
static IdentityType const value; \
};
	
	hm_declare_additive_identity(float, 0.f, float)
	hm_declare_additive_identity(double, 0., double)
	hm_declare_additive_identity(Value, 0., double)
	hm_declare_additive_identity(Point2d, Point2d(), Point2d)
	hm_declare_additive_identity(Point3d, Point3d(), Point3d)
	hm_declare_additive_identity(Skeleton3d, Skeleton3d(), Skeleton3d)
	hm_declare_additive_identity(Scene3d, Scene3d(), Scene3d)
	hm_declare_additive_identity(Image2d, 0, int)
	hm_declare_additive_identity(ListValue, ListValue(), ListValue)
	hm_declare_additive_identity(ListPoint2d, ListPoint2d(), ListPoint2d)
	hm_declare_additive_identity(ListPoint3d, ListPoint3d(), ListPoint3d)
	
	hm_declare_multiplicative_identity(float, 1.f, float)
	hm_declare_multiplicative_identity(double, 1., double)
	hm_declare_multiplicative_identity(Value, 1., double)
	hm_declare_multiplicative_identity(Point2d, Point2d(ci::Vec2f(1,1)), Point2d)
	hm_declare_multiplicative_identity(Point3d, Point3d(ci::Vec3f(1,1,1)), Point3d)
	hm_declare_multiplicative_identity(Skeleton3d, Skeleton3d(), Skeleton3d)
	hm_declare_multiplicative_identity(Scene3d, Scene3d(), Scene3d)
	hm_declare_multiplicative_identity(Image2d, 0, int)
	hm_declare_multiplicative_identity(ListValue, ListValue(), ListValue)
	hm_declare_multiplicative_identity(ListPoint2d, ListPoint2d(), ListPoint2d)
	hm_declare_multiplicative_identity(ListPoint3d, ListPoint3d(), ListPoint3d)


	
	
	
#define hm_enable_supports_addition(T, U, R) \
template<> \
struct supports_addition<T, U> : public std::true_type { \
	typedef R return_type; \
};
	
	
#define hm_enable_supports_commutative_addition(T, U, R) \
template<> \
struct supports_addition<T, U> : public std::true_type { \
	typedef R return_type; \
}; \
template<> \
struct supports_addition<U, T> : public std::true_type { \
	typedef R return_type; \
};
	
	hm_enable_supports_addition(double, double, double)
	hm_enable_supports_addition(float, float, float)
	hm_enable_supports_addition(Value, Value, Value)
	hm_enable_supports_addition(Point2d, Point2d, Point2d)
	hm_enable_supports_addition(Image2d, Image2d, Point2d)
	hm_enable_supports_addition(Point3d, Point3d, Point3d)
	hm_enable_supports_addition(Skeleton3d, Skeleton3d, Skeleton3d)
	hm_enable_supports_addition(Scene3d, Scene3d, Scene3d)
	hm_enable_supports_addition(ListValue, ListValue, ListValue)
	hm_enable_supports_addition(ListPoint2d, ListPoint2d, ListPoint2d)
	hm_enable_supports_addition(ListPoint3d, ListPoint3d, ListPoint3d)
	
	hm_enable_supports_commutative_addition(Value, Image2d, Image2d)
	hm_enable_supports_commutative_addition(Value, ListValue, ListValue)
	hm_enable_supports_commutative_addition(double, Image2d, Image2d)
	hm_enable_supports_commutative_addition(double, ListValue, ListValue)
	hm_enable_supports_commutative_addition(float, Image2d, Image2d)
	hm_enable_supports_commutative_addition(float, ListValue, ListValue)
	
	hm_enable_supports_commutative_addition(Point2d, ListPoint2d, ListPoint2d)
	hm_enable_supports_commutative_addition(Point3d, Skeleton3d, Skeleton3d)
	hm_enable_supports_commutative_addition(Point3d, Scene3d, Scene3d)
	hm_enable_supports_commutative_addition(Point3d, ListPoint3d, ListPoint3d)
	
	hm_enable_supports_commutative_addition(Skeleton3d, Scene3d, Scene3d)
	
	hm_enable_supports_commutative_addition(ListPoint3d, Skeleton3d, Scene3d)
	hm_enable_supports_commutative_addition(ListPoint3d, Scene3d, Scene3d)

	
	
	// define a type trait indicating T and U can be added/subtracted
#define hm_enable_supports_multiplication(T, U, R) \
template<> \
struct supports_multiplication<T, U> : public std::true_type \
{ \
typedef R return_type; \
}; \

	
#define hm_enable_supports_commutative_multiplication(T, U, R) \
template<> \
struct supports_multiplication<T, U> : public std::true_type \
{ \
typedef R return_type; \
}; \
template<> \
struct supports_multiplication<U, T> : public std::true_type \
{ \
typedef R return_type; \
};
	
	hm_enable_supports_multiplication(double, double, double)
	hm_enable_supports_multiplication(float, float, float)
	hm_enable_supports_multiplication(Value, Value, Value)
	hm_enable_supports_multiplication(Point2d, Point2d, Point2d)
	hm_enable_supports_multiplication(Image2d, Image2d, Image2d)
	hm_enable_supports_multiplication(Point3d, Point3d, Point3d)
	hm_enable_supports_multiplication(Skeleton3d, Skeleton3d, Skeleton3d)
	hm_enable_supports_multiplication(Scene3d, Scene3d, Scene3d)
	hm_enable_supports_multiplication(ListValue, ListValue, ListValue)
	hm_enable_supports_multiplication(ListPoint2d, ListPoint2d, ListPoint2d)
	hm_enable_supports_multiplication(ListPoint3d, ListPoint3d, ListPoint3d)
	
	hm_enable_supports_commutative_multiplication(Value, Image2d, Image2d)
	hm_enable_supports_commutative_multiplication(Value, ListValue, ListValue)
	hm_enable_supports_commutative_multiplication(Value, Point2d, Point2d)
	hm_enable_supports_commutative_multiplication(Value, ListPoint2d, ListPoint2d)
	hm_enable_supports_commutative_multiplication(Value, Point3d, Point3d)
	hm_enable_supports_commutative_multiplication(Value, ListPoint3d, ListPoint3d)
	hm_enable_supports_commutative_multiplication(Value, Skeleton3d, Skeleton3d)
	hm_enable_supports_commutative_multiplication(Value, Scene3d, Scene3d)
	hm_enable_supports_commutative_multiplication(double, Image2d, Image2d)
	hm_enable_supports_commutative_multiplication(double, ListValue, ListValue)
	hm_enable_supports_commutative_multiplication(double, Point2d, Point2d)
	hm_enable_supports_commutative_multiplication(double, ListPoint2d, ListPoint2d)
	hm_enable_supports_commutative_multiplication(double, Point3d, Point3d)
	hm_enable_supports_commutative_multiplication(double, ListPoint3d, ListPoint3d)
	hm_enable_supports_commutative_multiplication(double, Skeleton3d, Skeleton3d)
	hm_enable_supports_commutative_multiplication(double, Scene3d, Scene3d)
	hm_enable_supports_commutative_multiplication(float, Image2d, Image2d)
	hm_enable_supports_commutative_multiplication(float, ListValue, ListValue)
	hm_enable_supports_commutative_multiplication(float, Point2d, Point2d)
	hm_enable_supports_commutative_multiplication(float, ListPoint2d, ListPoint2d)
	hm_enable_supports_commutative_multiplication(float, Point3d, Point3d)
	hm_enable_supports_commutative_multiplication(float, ListPoint3d, ListPoint3d)
	hm_enable_supports_commutative_multiplication(float, Skeleton3d, Skeleton3d)
	hm_enable_supports_commutative_multiplication(float, Scene3d, Scene3d)
	
	
	hm_enable_supports_commutative_multiplication(Point2d, ListPoint2d, ListPoint2d)
	hm_enable_supports_commutative_multiplication(Point3d, ListPoint3d, ListPoint3d)
	hm_enable_supports_commutative_multiplication(Point3d, Skeleton3d, Skeleton3d)
	hm_enable_supports_commutative_multiplication(Point3d, Scene3d, Scene3d)
	
	hm_enable_supports_commutative_multiplication(Skeleton3d, Scene3d, Scene3d)
	
	hm_enable_supports_commutative_multiplication(ListPoint3d, Skeleton3d, Scene3d)
	hm_enable_supports_commutative_multiplication(ListPoint3d, Scene3d, Scene3d)


	
	
	static_assert(is_list<ListValue>::value && is_list<ListPoint2d>::value && is_list<ListPoint3d>::value && is_list<Scene3d>::value, "is_list type trait is incorrectly defined");
	static_assert(!(is_list<float>::value || is_list<Value>::value || is_list<Skeleton3d>::value), "is_list type trait is incorrectly defined");
	
	
#define hm_define_type_from_enum(typeEnum, Type) \
template<> struct type_from_enum<typeEnum> { typedef Type type; };
	
	hm_define_type_from_enum(UNDEFINED, DataNull)
	hm_define_type_from_enum(VALUE, Value)
	hm_define_type_from_enum(POINT2D, Point2d)
	hm_define_type_from_enum(POINT3D, Point3d)
	hm_define_type_from_enum(SKELETON3D, Skeleton3d)
	hm_define_type_from_enum(SCENE3D, Scene3d)
	hm_define_type_from_enum(IMAGE2D, Image2d)
	hm_define_type_from_enum(LIST_VALUE, ListValue)
	hm_define_type_from_enum(LIST_POINT2D, ListPoint2d)
	hm_define_type_from_enum(LIST_POINT3D, ListPoint3d)
	
	static_assert(ALL_TYPES == (UNDEFINED | VALUE | POINT2D | POINT3D | SKELETON3D | SCENE3D | IMAGE2D | LIST_VALUE | LIST_POINT2D | LIST_POINT3D), "type_from_enum is missing types.");

#define hm_declare_supports_maximum(Type) \
template <> struct supports_maximum<Type> : public std::true_type {};

	hm_declare_supports_maximum(float)
	hm_declare_supports_maximum(double)
	hm_declare_supports_maximum(Value)
	hm_declare_supports_maximum(ListValue)
	hm_declare_supports_maximum(Image2d)
	
#define hm_declare_supports_minimum(Type) \
	template <> struct supports_minimum<Type> : public std::true_type{};

	hm_declare_supports_minimum(float)
	hm_declare_supports_minimum(double)
	hm_declare_supports_minimum(Value)
	hm_declare_supports_minimum(ListValue)
	hm_declare_supports_minimum(Image2d)

}


				  
				  
				  
				  


