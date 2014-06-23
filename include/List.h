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
		List(double timestamp=0., int id=0, SceneMetaPtr sceneMeta=SceneMeta::sDefaultSceneMeta)
		: BaseType(timestamp, id, sceneMeta)
		{}
		
		List(std::vector<DataType> const& values, double timestamp=0., int id=0, SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta)
		: BaseType(timestamp, id, sceneMeta)
		, value(values)
		{}
		
		
		std::vector<DataType> value;
		
		virtual Type type() const override;
		virtual std::ostream& printTo(std::ostream&) const override;
		
		// defines for equality and inequality
		hm_data_define_equality_ops(List<DataType>)

		// Additionally, free operators for T * List<DataType>
		// are defined below for */+-
		
//		List<DataType> operator+() const;
		List<DataType> operator-() const;
		
		// Operations against types that may operate.
		// e.g. List<Type> * T -> List<Type*T>
		// on DataType. Only define if List<DataType> * T is defined and not
		// if T is a list
		// as we define those separately.
		// T is applied individually to each element of the list
		template <typename T>
		typename supports_addition<List<DataType>, T>::return_type
		operator+(T const& rhs) const;
		
		template <typename T>
		typename supports_addition<List<DataType>, T>::return_type
		operator-(T const& rhs) const;
		
		template <typename T>
		typename supports_multiplication<List<DataType>, T>::return_type
		operator*(T const& rhs) const;
		
		template <typename T>
		typename supports_multiplication<List<DataType>, T>::return_type
		operator/(T const& rhs) const;
		
		// Support an assignment operator if the operator is supported and
		// its return type
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
			std::is_same<typename supports_addition<List<DataType>, List<T>>::return_type, List<DataType>>::value,
			typename supports_addition<List<DataType>, T>::return_type&
		>::type
		operator+=(List<T> const& rhs);
		
		template <typename T>
		typename std::enable_if<
			std::is_same<typename supports_addition<List<DataType>, List<T>>::return_type, List<DataType>>::value,
			typename supports_addition<List<DataType>, T>::return_type&
		>::type
		operator-=(List<T> const& rhs);
		
		template <typename T>
		typename std::enable_if<
			std::is_same<typename supports_multiplication<List<DataType>, List<T>>::return_type, List<DataType>>::value,
			typename supports_multiplication<List<DataType>, T>::return_type&
		>::type
		operator*=(List<T> const& rhs);
		
		template <typename T>
		typename std::enable_if<
			std::is_same<typename supports_multiplication<List<DataType>, List<T>>::return_type, List<DataType>>::value,
			typename supports_multiplication<List<DataType>, T>::return_type&
		>::type
		operator/=(List<T> const& rhs);
		
		// Operators for List<DataType> on List<T> if DataType can operate on T.
		// List elements are operated piecewise
		
		template <typename T>
		typename supports_addition<List<DataType>, List<T>>::return_type
		operator+(List<T> const& rhs) const;
		
		template <typename T>
		typename supports_addition<List<DataType>, List<T>>::return_type
		operator-(List<T> const& rhs) const;
		
		template <typename T>
		typename supports_multiplication<List<DataType>, List<T>>::return_type
		operator*(List<T> const& rhs) const;
		
		template <typename T>
		typename supports_multiplication<List<DataType>, List<T>>::return_type
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
			++it;
		}
		return out << ']';
	}
	
	
	// MARK: Operators for types that can operate against DataType
	// e.g. List<Type> *= T
#define hm_define_list_T_assign(op_assign, trait) \
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
	
	hm_define_list_T_assign(+=, supports_addition)
	hm_define_list_T_assign(-=, supports_addition)
	hm_define_list_T_assign(*=, supports_multiplication)
	hm_define_list_T_assign(/=, supports_multiplication)
	
	// e.g. List<Type> * T
#define hm_define_list_T_op(op, trait) \
	template <typename DataType> \
	template <typename T> \
typename trait<List<DataType>, T>::return_type \
	List<DataType>::operator op(T const& rhs) const \
	{ \
		typename trait<List<DataType>, T>::return_type out; \
		out.value.reserve(value.size()); \
		for (auto it=value.begin(); it!=value.end(); ++it) \
		{ \
			out.value.push_back(*it op rhs); \
		} \
		return out; \
	}
	
	hm_define_list_T_op(+, supports_addition)
	hm_define_list_T_op(-, supports_addition)
	hm_define_list_T_op(*, supports_multiplication)
	hm_define_list_T_op(/, supports_multiplication)
	
// e.g. List<Type> *= List<T> -> List<Type> piecewise
#define hm_define_list_listT_op_assign(op_assign, trait) \
	template <typename DataType> \
	template <typename T> \
		typename std::enable_if< \
			std::is_same<typename trait<List<DataType>, List<T>>::return_type, List<DataType>>::value, \
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
	
	
	// e.g. List<Type> * List<T> -> List<Type * T>
#define hm_define_list_listT_op(op, trait) \
	template <typename DataType> \
	template <typename T> \
	typename trait <List<DataType>, List<T>>::return_type \
	List<DataType>::operator op(List<T> const& rhs) const \
	{ \
		typename trait <List<DataType>, List<T>>::return_type out; \
		int n = std::min(value.size(), rhs.value.size()); \
		for (int i=0; i<n; i++) \
		{ \
			out.value.push_back(value[i] op rhs.value[i]); \
		} \
		return out; \
	}
	
	hm_define_list_listT_op(+, supports_addition)
	hm_define_list_listT_op(-, supports_addition)
	hm_define_list_listT_op(*, supports_multiplication)
	hm_define_list_listT_op(/, supports_multiplication)
	
	
	// -List<Type> -> List<Type>
#define hm_define_list_unary_op(op) \
	template <typename DataType> \
	List<DataType> List<DataType>::operator op() const \
	{ \
		List<DataType> out; \
		out.value.reserve(value.size()); \
		for (DataType const& x: value) \
		{ \
			out.value.push_back(op x); \
		} \
		return out; \
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

}











