#pragma once
#include "Common.h"
#include "SceneMeta.h"
#include <vector>
#include <type_traits>
#include <utility>
#include "Value.h"
#include <boost/utility/enable_if.hpp>

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

//		// Individual elements are added to each other.
//		// If lists are of different sizes then the output is the length
//		// of the shorter list
//		List<DataType> operator+(List<DataType> const& rhs) const;
//		List<DataType> operator-(List<DataType> const& rhs) const;
//		List<DataType> operator*(List<DataType> const& rhs) const;
//		List<DataType> operator/(List<DataType> const& rhs) const;
//		List<DataType>& operator+=(List<DataType> const& rhs);
//		List<DataType>& operator-=(List<DataType> const& rhs);
//		List<DataType>& operator*=(List<DataType> const& rhs);
//		List<DataType>& operator/=(List<DataType> const& rhs);
//		
//		// Element is applied to each element of the list
//		List<DataType> operator+(DataType const& rhs) const;
//		List<DataType> operator-(DataType const& rhs) const;
//		List<DataType> operator*(DataType const& rhs) const;
//		List<DataType> operator/(DataType const& rhs) const;
//		List<DataType>& operator+=(DataType const& rhs);
//		List<DataType>& operator-=(DataType const& rhs);
//		List<DataType>& operator*=(DataType const& rhs);
//		List<DataType>& operator/=(DataType const& rhs);
		
		// defines for equality and inequality
		hm_data_define_equality_ops(List<DataType>)

		// Additionally, free operators for DataType * List<DataType>
		// are defined below for */+-
		// as well as for List<DataType> * ScalarType, List<DataType> *=
		// ScalarType and ScalarType * List
		
//		List<DataType> operator+() const;
		List<DataType> operator-() const;
		
		// Operations against types that may operate.
		// e.g. List<Type> * T -> List<Type*T>
		// on DataType. Only define if List<DataType> * T is defined and not if T is a list
		// as we define those separately.
		// T is applied individually to each element of the list
		template <typename T>
		typename supports_addition<List<DataType>, typename boost::disable_if<is_list<T>, T>>::return_type
		operator+(T const& rhs) const;
		
		template <typename T>
		typename supports_addition<List<DataType>, typename boost::disable_if<is_list<T>, T>>::return_type
		operator-(T const& rhs) const;
		
		template <typename T>
		typename supports_multiplication<List<DataType>, typename boost::disable_if<is_list<T>, T>>::return_type
		operator*(T const& rhs) const;
		
		template <typename T>
		typename supports_multiplication<List<DataType>, typename boost::disable_if<is_list<T>, T>>::return_type
		operator/(T const& rhs) const;
		
		// Support an assignment operator if the operator is supported and its return type
		// equals that of this class, and if T is not a list
		// e.g. List<DataType> *= T -> List<DataType>
		template <typename T>
		typename std::enable_if<
			std::is_same<
				List<DataType>,
				typename supports_addition<
					List<DataType>,
					typename boost::disable_if<is_list<T>, T>::type
				>::return_type>::value, typename supports_addition<List<DataType>, T>::return_type&>
		::type
		operator+=(T const& rhs);
		
		template <typename T>
		typename std::enable_if<
			std::is_same<
				List<DataType>,
				typename supports_addition<
					List<DataType>,
					typename boost::disable_if<is_list<T>, T>::type
				>::return_type>::value, typename supports_addition<List<DataType>, T>::return_type&>
		::type
		operator-=(T const& rhs);
		
		template <typename T>
		typename std::enable_if<
			std::is_same<
				List<DataType>,
				typename supports_multiplication<
					List<DataType>,
					typename boost::disable_if<is_list<T>, T>::type
				>::return_type>::value, typename supports_multiplication<List<DataType>, T>::return_type&>
		::type
		operator*=(T const& rhs);
		
		template <typename T>
		typename std::enable_if<
			std::is_same<
				List<DataType>,
				typename supports_multiplication<
					List<DataType>,
					typename boost::disable_if<is_list<T>, T>::type
				>::return_type>::value, typename supports_multiplication<List<DataType>, T>::return_type&>
		::type
		operator/=(T const& rhs);
		
		// Operations against lists of types that may operate
		// on DataType. The length of the resulting list is the
		// minimum of this list and the other list.
		// Enable if T is a list, List<DataType> and List<T> support the operation, and
		// the result of the operation is List<DataType>
		template <typename T>
		typename std::enable_if<
			is_list<T>::value && std::is_same<typename supports_addition<List<DataType>, T>::return_type, List<DataType>>::value,
			typename supports_addition<List<DataType>, T>::return_type&
		>::type
		operator+=(List<T> const& rhs);
		
		template <typename T>
		typename std::enable_if<
			is_list<T>::value && std::is_same<typename supports_addition<List<DataType>, T>::return_type, List<DataType>>::value,
			typename supports_addition<List<DataType>, T>::return_type&
		>::type
		operator-=(List<T> const& rhs);
		
		template <typename T>
		typename std::enable_if<
			is_list<T>::value && std::is_same<typename supports_multiplication<List<DataType>, T>::return_type, List<DataType>>::value,
			typename supports_multiplication<List<DataType>, T>::return_type&
		>::type
		operator*=(List<T> const& rhs);
		
		template <typename T>
		typename std::enable_if<
			is_list<T>::value && std::is_same<typename supports_multiplication<List<DataType>, T>::return_type, List<DataType>>::value,
			typename supports_multiplication<List<DataType>, T>::return_type&
		>::type
		operator/=(List<T> const& rhs);
		
		// Operators for List<DataType> on List<T> if DataType can operate on T.
		// List elements are operated piecewise
		
		template <typename T>
		typename std::enable_if<is_list<T>::value, typename supports_addition<List<DataType>, T>::return_type>::type
		operator+(List<T> const& rhs) const;
		
		template <typename T>
		typename std::enable_if<is_list<T>::value, typename supports_addition<List<DataType>, T>::return_type>::type
		operator-(List<T> const& rhs) const;
		
		template <typename T>
		typename std::enable_if<is_list<T>::value, typename supports_multiplication<List<DataType>, T>::return_type>::type
		operator*(List<T> const& rhs) const;
		
		template <typename T>
		typename std::enable_if<is_list<T>::value, typename supports_multiplication<List<DataType>, T>::return_type>::type
		operator/(List<T> const& rhs) const;

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
	
//	// e.g. List<Type> *= List<Type>
//#define hm_define_list_list_assign(op_assign) \
//	template <typename DataType> \
//	List<DataType>& List<DataType>:: operator op_assign (List<DataType> const& rhs) \
//	{ \
//		int n = std::min(value.size(), rhs.value.size()); \
//		value.resize(n); \
//		for (int i=0; i<n; i++) \
//		{ \
//			value[i] op_assign rhs.value[i]; \
//		} \
//		return *this; \
//	}
//	
//	hm_define_list_list_assign(+=)
//	hm_define_list_list_assign(-=)
//	hm_define_list_list_assign(*=)
//	hm_define_list_list_assign(/=)
//	
//	
//	// e.g. List<Type> * List<Type>
//#define hm_define_list_list_op(op) \
//	template <typename DataType> \
//	List<DataType> List<DataType>:: operator op (List<DataType> const& rhs) const \
//	{ \
//		return List<DataType>(*this) op##= rhs; \
//	}
//	
//	hm_define_list_list_op(+)
//	hm_define_list_list_op(-)
//	hm_define_list_list_op(*)
//	hm_define_list_list_op(/)
	
	// MARK: Operators for types that can operate against DataType
	// e.g. List<Type> *= T
#define hm_define_list_datatype_assign(op_assign, trait) \
	template <typename DataType> \
	template <typename T> \
		typename std::enable_if< \
			std::is_same< \
				List<DataType>, \
				typename trait< \
					List<DataType>, \
					typename boost::disable_if<is_list<T>, T>::type \
				>::return_type>::value, typename trait<List<DataType>, T>::return_type&> \
		::type \
	List<DataType>:: operator op_assign(T const& rhs) \
	{ \
		for (DataType& x: value) \
		{ \
			x op_assign rhs; \
		} \
		return *this; \
	}
	
	hm_define_list_datatype_assign(+=, supports_addition)
	hm_define_list_datatype_assign(-=, supports_addition)
	hm_define_list_datatype_assign(*=, supports_multiplication)
	hm_define_list_datatype_assign(/=, supports_multiplication)
	
	// e.g. List<Type> * T
#define hm_define_list_datatype_op(op, trait) \
	template <typename DataType> \
	template <typename T> \
	typename trait<List<DataType>, typename boost::disable_if<is_list<T>, T>>::return_type \
	List<DataType>::operator op(T const& rhs) const \
	{ \
		return List<DataType>(*this) op##= rhs; \
	}
	
	hm_define_list_datatype_op(+, supports_addition)
	hm_define_list_datatype_op(-, supports_addition)
	hm_define_list_datatype_op(*, supports_multiplication)
	hm_define_list_datatype_op(/, supports_multiplication)
	
// e.g. List<Type> *= List<T> piecewise
#define hm_define_list_listT_op_assign(op_assign, trait) \
	template <typename DataType> \
	template <typename T> \
		typename std::enable_if< \
			is_list<T>::value && std::is_same<typename trait<List<DataType>, T>::return_type, List<DataType>>::value, \
			typename trait<List<DataType>, T>::return_type& \
		>::type \
	List<DataType>::operator op_assign(List<T> const& rhs) \
	{ \
		value.resize(std::min(value.size(), rhs.value.size())); \
		auto lhs_it = value.begin(); \
		auto rhs_it = rhs.value.begin(); \
		for ( ; lhs_it != value.end(); ++lhs_it, ++rhs_it) \
		{ \
			*lhs_it op_assign *rhs_it; \
		} \
		return *this; \
	}
	
	hm_define_list_listT_op_assign(+=, supports_addition)
	hm_define_list_listT_op_assign(-=, supports_addition)
	hm_define_list_listT_op_assign(*=, supports_multiplication)
	hm_define_list_listT_op_assign(/=, supports_multiplication)
	
	
	// e.g. List<Type> * List<T>
#define hm_define_list_listT_op(op, trait) \
	template <typename DataType> \
	template <typename T> \
typename std::enable_if<is_list<T>::value, typename trait<List<DataType>, T>::return_type>::type \
	List<DataType>::operator op(List<T> const& rhs) const \
	{ \
		return (typename trait<List<DataType>, T>::return_type)(*this) op##= rhs; \
	}
	
	hm_define_list_listT_op(+, supports_addition)
	hm_define_list_listT_op(-, supports_addition)
	hm_define_list_listT_op(*, supports_multiplication)
	hm_define_list_listT_op(/, supports_multiplication)
	
	
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

	
	// MARK: Free operators with container right hand side
	
	// e.g. T * List<Type> if T is not a list and T * Type is defined
#define hm_define_T_list_op(op, trait) \
template <typename T, typename DataType> \
typename boost::disable_if<is_list<T>, typename trait<T, List<DataType>>::return_type>::type \
operator op (T const& lhs, List<DataType> const& rhs) \
{ \
	static_assert(trait<T, List<DataType>>::value, "SFINAE failure"); \
	typename trait<T, List<DataType>>::return_type out; \
	out.value.reserve(rhs.value.size()); \
	for (auto it = rhs.value.begin(); it!=rhs.value.end(); ++it) \
	{ \
		out.value.push_back(lhs op *it); \
	} \
	return out; \
}
	
	hm_define_T_list_op(+, supports_addition)
	hm_define_T_list_op(-, supports_addition)
	hm_define_T_list_op(*, supports_multiplication)
	hm_define_T_list_op(/, supports_multiplication)

//	// Non-commutative operators require a bit more work
//	
//	template <typename DataType>
//	List<DataType> operator- (DataType const& lhs, List<DataType> rhs)
//	{
//		for (DataType& x: rhs.value)
//		{
//			x = lhs - x;
//		}
//	}
//
//	template <typename DataType>
//	List<DataType> operator/ (DataType const& lhs, List<DataType> rhs)
//	{
//		for (DataType& x: rhs.value)
//		{
//			x = lhs / x;
//		}
//	}
//
//	// MARK: Operators for scalars
//	
//	// The return types on these use type_traits to make the compiler ignore the definitions
//	// for certain DataTypes. This is to prevent addition/subtraction operators being defined
//	// when the underlying DataType does not support them.
//
//	
//	// Non-assigning operators on the right hand side:
//	
//	// Helper:
//#define _hm_list_define_free_op_value(op, is_op_valid_for_type) \
//	template <typename DataType> \
//	typename std::enable_if<is_op_valid_for_type, List<DataType>>::type \
//	operator op (List<DataType> lhs, Value const& rhs) \
//	{ \
//		for (DataType & x: lhs.value) \
//		{ \
//			x op##= rhs.value; \
//		} \
//		return lhs; \
//	}
//
//	// Helper:
//#define _hm_list_define_free_op_single_scalar(op, ScalarType, is_op_valid_for_type) \
//	template <typename DataType> \
//	typename std::enable_if<is_op_valid_for_type, List<DataType>>::type \
//	operator op (List<DataType> lhs, ScalarType rhs) \
//	{ \
//		for (DataType & x: lhs.value) \
//		{ \
//			 x op##= rhs; \
//		} \
//		return lhs; \
//	}
//	
//#define hm_list_define_free_op_scalars(op, is_op_valid_for_type) \
//	_hm_list_define_free_op_value(op, is_op_valid_for_type) \
//	_hm_list_define_free_op_single_scalar(op, float, is_op_valid_for_type) \
//	_hm_list_define_free_op_single_scalar(op, double, is_op_valid_for_type) \
//	
//	// Now the actual definitions
//	hm_list_define_free_op_scalars(+, DataType::supports_scalar_addition)
//	hm_list_define_free_op_scalars(-, DataType::supports_scalar_addition)
//	hm_list_define_free_op_scalars(*, true)
//	hm_list_define_free_op_scalars(/, true)
//	
////	// Assignment operators on the right hand side:
//	// Removed as these are now covered by the above List<D> * T operators
////	
////	
////	// Helper:
////#define _hm_list_define_free_op_assign_value(op_assign, is_op_valid_for_type) \
////template <typename DataType> \
////typename std::enable_if<is_op_valid_for_type, List<DataType>&>::type \
////operator op_assign(List<DataType> & lhs, Value rhs) \
////{ \
////for (DataType & x: lhs.value) \
////{ \
////x op_assign rhs.value; \
////} \
////return lhs; \
////}
////
////	
////#define _hm_list_define_free_op_assign_single_scalar(op_assign, ScalarType, is_op_valid_for_type) \
////	template <typename DataType> \
////	typename std::enable_if<is_op_valid_for_type, List<DataType>&>::type \
////	operator op_assign(List<DataType> & lhs, ScalarType rhs) \
////	{ \
////		for (DataType& x: lhs.value) \
////		{ \
////			x op_assign rhs; \
////		} \
////		return lhs; \
////	}
////	
////#define hm_list_define_free_op_assign_scalars(op_assign, is_op_valid_for_type) \
////_hm_list_define_free_op_assign_value(op_assign, is_op_valid_for_type) \
////_hm_list_define_free_op_assign_single_scalar(op_assign, float, is_op_valid_for_type) \
////_hm_list_define_free_op_assign_single_scalar(op_assign, double, is_op_valid_for_type) \
////
////	// Now the actual definitions
////	hm_list_define_free_op_assign_scalars(+=, DataType::supports_scalar_addition)
////	hm_list_define_free_op_assign_scalars(-=, DataType::supports_scalar_addition)
////	hm_list_define_free_op_assign_scalars(*=, true)
////	hm_list_define_free_op_assign_scalars(/=, true)
//	
//	
//	// Non-assigning operators on the left hand side
//	// e.g. T * List<DataType> for when T * DataType is defined
//#define hm_list_define_free_left_op(op, trait) \
//template <typename DataType, typename T> \
//typename std::enable_if<trait<T, DataType>::value, List<DataType>>::type \
//operator op(T const& lhs, List<DataType> rhs) \
//{ \
//for (DataType & x: rhs.value) \
//{ \
//x = lhs op x; \
//} \
//return rhs; \
//}
//	
////#define _hm_list_define_free_left_op_single_scalar(op, ScalarType, is_op_valid_for_type) \
////template <typename DataType> \
////typename std::enable_if<is_op_valid_for_type, List<DataType>>::type \
////operator op(ScalarType lhs, List<DataType> rhs) \
////{ \
////for (DataType & x: rhs.value) \
////{ \
////x = lhs op x; \
////} \
////return rhs; \
////}
////	
////#define hm_list_define_free_left_op_scalars(op, is_op_valid_for_type) \
////	_hm_list_define_free_left_op_value(op, is_op_valid_for_type) \
////	_hm_list_define_free_left_op_single_scalar(op, float, is_op_valid_for_type) \
////	_hm_list_define_free_left_op_single_scalar(op, double, is_op_valid_for_type)
//	
//	// Now the actual definitions
//	hm_list_define_free_left_op(*, supports_multiplication)
//	hm_list_define_free_left_op(/, supports_multiplication)
//	hm_list_define_free_left_op(+, supports_addition)
//	hm_list_define_free_left_op(-, supports_addition)


}











