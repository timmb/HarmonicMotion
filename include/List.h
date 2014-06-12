#pragma once
#include "Common.h"
#include "SceneMeta.h"
#include <vector>
#include <type_traits>
#include <utility>
#include "Value.h"

namespace hm
{
	/// Base class for List types
	template <typename DataType>
	class List : public DataType::BaseType
	{
	public:
		typedef typename DataType::BaseType BaseType;
		
		explicit
		List(double timestamp=0., SceneMetaPtr sceneMeta=SceneMeta::sDefaultSceneMeta)
		: BaseType(timestamp, sceneMeta)
		{}
		
		List(std::vector<DataType> const& values, double timestamp=0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta)
		: BaseType(timestamp, sceneMeta)
		, value(values)
		{}
		
		
		std::vector<DataType> value;
		
		virtual Type type() const override;
		virtual std::ostream& printTo(std::ostream&) const override;

		// Individual elements are added to each other.
		// If lists are of different sizes then the output is the length
		// of the shorter list
		List<DataType> operator+(List<DataType> const& rhs) const;
		List<DataType> operator-(List<DataType> const& rhs) const;
		List<DataType> operator*(List<DataType> const& rhs) const;
		List<DataType> operator/(List<DataType> const& rhs) const;
		List<DataType>& operator+=(List<DataType> const& rhs);
		List<DataType>& operator-=(List<DataType> const& rhs);
		List<DataType>& operator*=(List<DataType> const& rhs);
		List<DataType>& operator/=(List<DataType> const& rhs);
		
		// Element is applied to each element of the list
		List<DataType> operator+(DataType const& rhs) const;
		List<DataType> operator-(DataType const& rhs) const;
		List<DataType> operator*(DataType const& rhs) const;
		List<DataType> operator/(DataType const& rhs) const;
		List<DataType>& operator+=(DataType const& rhs);
		List<DataType>& operator-=(DataType const& rhs);
		List<DataType>& operator*=(DataType const& rhs);
		List<DataType>& operator/=(DataType const& rhs);
		
		// defines for equality and inequality
		hm_data_define_equality_ops(List<DataType>)

		// Additionally, free operators for DataType * List<DataType>
		// are defined below for */+-
		// as well as for List<DataType> * ScalarType, List<DataType> *=
		// ScalarType and ScalarType * List
		
//		List<DataType> operator+() const;
		List<DataType> operator-() const;
		
	};
	
	template <typename DataType>
	Type List<DataType>::type() const
	{
		return getType<List<DataType>>();
	}

	template <typename DataType>
	std::ostream& List<DataType>::printTo(std::ostream& out) const
	{
		out << '[';
		auto it = begin(value);
		if (it!=end(value))
		{
			out << *it++;
		}
		while (it!=end(value))
		{
			out << ", "<<*it;
		}
		return out << ']';
	}
	
//	// MARK: Helper Macros for derived types
//	
//	
//	// -------------------- e.g. List<Type> * Scalar
//	// Macros to define operations against Scalars where
//	// l * x = [t * x] for t in l where l is List<T>, t is T and x is Scalar
//	// use hm_list_define_mult_scalar_operators_and_assigns(Type)
//	
//	// Helper:
//#define hm_list_define_single_scalar_assign(Type, assign_op, Scalar) \
//	inline Type& assign_op (Scalar rhs) \
//	{ \
//	for (auto& x: value) \
//	{ \
//	x.assign_op(rhs); \
//	} \
//	return *this; \
//	}
//	
//	// Helper: Define assignment operator for all scalars types
//#define hm_list_define_scalar_assign(Type, assign_op) \
//	hm_list_define_single_scalar_assign(Type, assign_op, Value const&) \
//	hm_list_define_single_scalar_assign(Type, assign_op, float) \
//	hm_list_define_single_scalar_assign(Type, assign_op, double)
//	
//#define hm_list_define_single_scalar_op(Type, op, Scalar) \
//	inline Type op (Scalar rhs) const\
//	{ \
//	return Type(*this).op##=(rhs); \
//	}
//	
//	// Helper: Define normal operator for all scalar types
//#define hm_list_define_scalar_op(Type, op) \
//	hm_list_define_single_scalar_op(Type, op, Value const&) \
//	hm_list_define_single_scalar_op(Type, op, float) \
//	hm_list_define_single_scalar_op(Type, op, double) \
//
//	// *
//	// Define operators and assigns against Scalars just for * and /
//	// Place inside class body
//#define hm_list_define_mult_scalar_operators_and_assigns(Type) \
//	hm_list_define_scalar_assign(Type, operator*=) \
//	hm_list_define_scalar_assign(Type, operator/=) \
//	hm_list_define_scalar_op(Type, operator*) \
//	hm_list_define_scalar_op(Type, operator/)
//		
//	// *
//	// To define operators and assignes agianst Scalars for *, /, +, -
//	// Place inside class body
//#define hm_list_define_all_scalar_operators_and_assigns(Type) \
//	hm_list_define_scalar_assign(Type, operator+=) \
//	hm_list_define_scalar_assign(Type, operator-=) \
//	hm_list_define_scalar_op(Type, operator+) \
//	hm_list_define_scalar_op(Type, operator-) \
//	hm_list_define_mult_scalar_operators_and_assigns(Type)
//	
//	// -------------- e.g. Scalar * List<Type>
//	
//	// Helper: Define free operator for Scalar against op_func
//#define hm_list_define_single_free_scalar_op(Type, op_func, Scalar) \
//	inline Type op_func (Scalar x, Type rhs) \
//	{ \
//		for (auto & y: rhs.value) \
//		{ \
//			y = op_func(x, y); \
//		} \
//		return rhs; \
//	}
//	
//	// Helper: Define free operator for Value const&, float, double against op_func
//	// TODO: These should set the timestamp for Value types
//#define hm_list_define_free_mult_scalar_op(Type, op_func) \
//	hm_list_define_single_free_scalar_op(Type, op_func, Value const&) \
//	hm_list_define_single_free_scalar_op(Type, op_func, float) \
//	hm_list_define_single_free_scalar_op(Type, op_func, double)
//	
//	// *
//	// For, e.g. List<Point3d>:
//	// Define free multiplication operators (*/) for Value const&, float, double
//	// Place outside class body
//#define hm_list_define_free_mult_scalar_operators(Type) \
//	hm_list_define_free_mult_scalar_op(Type, operator*) \
//	hm_list_define_free_mult_scalar_op(Type, operator/)
//	
//	// *
//	// For, e.g. List<Value>:
//	// Define all free operators (*/+-) for Value const&, float, double
//	// Place outside class body
//#define hm_list_define_free_scalar_operators(Type) \
//	hm_list_define_free_mult_scalar_operators(Type) \
//	hm_list_define_free_mult_scalar_op(Type, operator+) \
//	hm_list_define_free_mult_scalar_op(Type, operator-)
	

	
	// --------------------------------------------------
	// --------------------------------------------------
	// --------------------------------------------------
	
	// MARK: Template definitions for the above class
	
	// e.g. List<Type> *= List<Type>
#define hm_define_list_list_assign(op_assign) \
	template <typename DataType> \
	List<DataType>& List<DataType>:: operator op_assign (List<DataType> const& rhs) \
	{ \
		int n = std::min(value.size(), rhs.value.size()); \
		for (int i=0; i<n; i++) \
		{ \
			value[i] op_assign rhs.value[i]; \
		} \
		return *this; \
	}
	
	hm_define_list_list_assign(+=)
	hm_define_list_list_assign(-=)
	hm_define_list_list_assign(*=)
	hm_define_list_list_assign(/=)
	
	
	// e.g. List<Type> * List<Type>
#define hm_define_list_list_op(op) \
	template <typename DataType> \
	List<DataType> List<DataType>:: operator op (List<DataType> const& rhs) const \
	{ \
		List<DataType> out = *this; \
		return out op##= rhs; \
	}
	
	hm_define_list_list_op(+)
	hm_define_list_list_op(-)
	hm_define_list_list_op(*)
	hm_define_list_list_op(/)
	
	// e.g. List<Type> *= Type
#define hm_define_list_datatype_assign(op_assign) \
	template <typename DataType> \
	List<DataType>& List<DataType>:: operator op_assign(DataType const& rhs) \
	{ \
		for (DataType& x: value) \
		{ \
			x op_assign rhs; \
		} \
		return *this; \
	}
	
	hm_define_list_datatype_assign(+=)
	hm_define_list_datatype_assign(-=)
	hm_define_list_datatype_assign(*=)
	hm_define_list_datatype_assign(/=)
	
	// e.g. List<Type> * Type
#define hm_define_list_datatype_op(op) \
	template <typename DataType> \
	List<DataType> List<DataType>::operator op(DataType const& rhs) const \
	{ \
		List<DataType> out = *this; \
		return out op##= rhs; \
	}
	
	hm_define_list_datatype_op(+)
	hm_define_list_datatype_op(-)
	hm_define_list_datatype_op(*)
	hm_define_list_datatype_op(/)
	

	// -List<Type>
#define hm_define_list_unary_op(op) \
	template <typename DataType> \
	List<DataType> List<DataType>::operator op() const \
	{ \
		List<DataType> out; \
		out.reserve(value.size()); \
		for (DataType const& x: value) \
		{ \
			out.value.push_back(op x); \
		} \
	}

//	hm_define_list_unary_op(+, operator+)
	hm_define_list_unary_op(-)

	
	// MARK: Free operators with container left hand side
	
	// e.g. Type * List<Type>
#define hm_define_datatype_list_op(op) \
template <typename DataType> \
List<DataType> operator op (DataType const& lhs, List<DataType> const& rhs) \
{ \
return rhs op lhs; \
}
	
	hm_define_datatype_list_op(+)
	hm_define_datatype_list_op(*)

	// Non-commutative operators require a bit more work
	
	template <typename DataType>
	List<DataType> operator- (DataType const& lhs, List<DataType> rhs)
	{
		for (DataType& x: rhs.value)
		{
			x = lhs - x;
		}
	}

	template <typename DataType>
	List<DataType> operator/ (DataType const& lhs, List<DataType> rhs)
	{
		for (DataType& x: rhs.value)
		{
			x = lhs / x;
		}
	}

	// MARK: Operators for scalars
	
	// This horrendous looking return type:
	// typename std::enable_if<std::is_same<decltype(DataType() op_assign Value()),decltype(DataType() op_assign Value())>::value, List<DataType>&>::type
	// is equivalent to
	// List<DataType>&
	// but disables the template if DataType
	// does not support assignment by Value.

	
	// Non-assigning operators on the right hand side:
	
	// Helper:
#define _hm_list_define_free_op_value(op) \
	template <typename DataType> \
	typename std::enable_if<std::is_same<decltype(DataType() op Value()),decltype(DataType() op Value())>::value, List<DataType>>::type \
	operator op (List<DataType> lhs, Value const& rhs) \
	{ \
		for (DataType & x: lhs.value) \
		{ \
			x op##= rhs.value; \
		} \
		return lhs; \
	}

	// Helper:
#define _hm_list_define_free_op_single_scalar(op, ScalarType) \
	template <typename DataType> \
	typename std::enable_if<std::is_same<decltype(DataType() op ScalarType()),decltype(DataType() op ScalarType())>::value, List<DataType>>::type \
	operator op (List<DataType> lhs, ScalarType rhs) \
	{ \
		for (DataType & x: lhs.value) \
		{ \
			 x op##= rhs; \
		} \
		return lhs; \
	}
	
#define hm_list_define_free_op_scalars(op) \
	_hm_list_define_free_op_value(op) \
	_hm_list_define_free_op_single_scalar(op, float) \
	_hm_list_define_free_op_single_scalar(op, double) \
	
	// Now the actual definitions
	hm_list_define_free_op_scalars(+)
	hm_list_define_free_op_scalars(-)
	hm_list_define_free_op_scalars(*)
	hm_list_define_free_op_scalars(/)
	
	// Assignment operators on the right hand side:
	
	
	// Helper:
#define _hm_list_define_free_op_assign_value(op_assign) \
template <typename DataType> \
typename std::enable_if<std::is_same<decltype(DataType() op_assign Value()),decltype(DataType() op_assign Value())>::value, List<DataType>&>::type \
operator op_assign(List<DataType> & lhs, Value rhs) \
{ \
for (DataType & x: lhs.value) \
{ \
x op_assign rhs.value; \
} \
return lhs; \
}

	
#define _hm_list_define_free_op_assign_single_scalar(op_assign, ScalarType) \
	template <typename DataType> \
	typename std::enable_if<std::is_same<decltype(DataType() op_assign ScalarType()),decltype(DataType() op_assign ScalarType())>::value, List<DataType>&>::type \
	operator op_assign(List<DataType> & lhs, ScalarType rhs) \
	{ \
		for (DataType& x: lhs.value) \
		{ \
			x op_assign rhs; \
		} \
		return lhs; \
	}
	
#define hm_list_define_free_op_assign_scalars(op_assign) \
_hm_list_define_free_op_assign_value(op_assign) \
_hm_list_define_free_op_assign_single_scalar(op_assign, float) \
_hm_list_define_free_op_assign_single_scalar(op_assign, double) \

	// Now the actual definitions
	hm_list_define_free_op_assign_scalars(+=)
	hm_list_define_free_op_assign_scalars(-=)
	hm_list_define_free_op_assign_scalars(*=)
	hm_list_define_free_op_assign_scalars(/=)
	
	
	// Non-assigning operators on the left hand side
#define _hm_list_define_free_left_op_value(op) \
template <typename DataType> \
typename std::enable_if<std::is_same<decltype(DataType() op Value()),decltype(DataType() op Value())>::value, List<DataType>>::type \
operator op(Value const& lhs, List<DataType> rhs) \
{ \
for (DataType & x: rhs.value) \
{ \
x = lhs.value op x; \
} \
return rhs; \
}
	
#define _hm_list_define_free_left_op_single_scalar(op, ScalarType) \
template <typename DataType> \
typename std::enable_if<std::is_same<decltype(DataType() op ScalarType()),decltype(DataType() op ScalarType())>::value, List<DataType>>::type \
operator op(ScalarType lhs, List<DataType> rhs) \
{ \
for (DataType & x: rhs.value) \
{ \
x = lhs op x; \
} \
return rhs; \
}
	
#define hm_list_define_free_left_op_scalars(op) \
	_hm_list_define_free_left_op_value(op) \
	_hm_list_define_free_left_op_single_scalar(op, float) \
	_hm_list_define_free_left_op_single_scalar(op, double)
	
	// Now the actual definitions
	hm_list_define_free_left_op_scalars(*)
	hm_list_define_free_left_op_scalars(/)
	hm_list_define_free_left_op_scalars(+)
	hm_list_define_free_left_op_scalars(-)
}











