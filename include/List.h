#pragma once
#include "Common.h"
#include "SceneMeta.h"
#include <vector>

namespace hm
{
	/// Base class for List types
	template <typename DataType>
	class List : public DataType::BaseType
	{
	public:
		typedef typename DataType::BaseType BaseType;
		
		List(double timestamp, SceneMetaPtr sceneMeta)
		: BaseType(timestamp, sceneMeta)
		{}
		
		std::vector<DataType> value;
		
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

		
		List<DataType> operator+() const;
		List<DataType> operator-() const;
		
		
	};
	
	
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
	
	
	
#define hm_define_list_list_assign(op, op_func) \
	template <typename DataType> \
	List<DataType>& List<DataType>:: op_func (List<DataType> const& rhs) \
	{ \
		int n = min(value.size(), rhs.value.size()); \
		for (int i=0; i<n; i++) \
		{ \
			value[i] op rhs.value[i]; \
		} \
		return rhs; \
	}
	
	hm_define_list_list_assign(+, operator+=)
	hm_define_list_list_assign(-, operator-=)
	hm_define_list_list_assign(*, operator*=)
	hm_define_list_list_assign(/, operator/=)
	
	
#define hm_define_list_list_op(op_assign, op_func) \
	template <typename DataType> \
	List<DataType> List<DataType>:: op_func (List<DataType> const& rhs) const \
	{ \
		List<DataType> out = *this; \
		return out op_assign rhs; \
	}
	
	hm_define_list_list_op(+=, operator+)
	hm_define_list_list_op(-=, operator-)
	hm_define_list_list_op(*=, operator*)
	hm_define_list_list_op(/=, operator/)
	
	
#define hm_define_list_datatype_assign(op_assign, op_func) \
	template <typename DataType> \
	List<DataType>& List<DataType>::op_func(DataType const& rhs) \
	{ \
		for (DataType& x: value) \
		{ \
			x op_assign rhs; \
		} \
	return *this; \
	}
	
	hm_define_list_datatype_assign(+=, operator+=)
	hm_define_list_datatype_assign(-=, operator-=)
	hm_define_list_datatype_assign(*=, operator*=)
	hm_define_list_datatype_assign(/=, operator/=)
	
	
#define hm_define_list_datatype_op(op_assign, op_func) \
	template <typename DataType> \
	List<DataType> List<DataType>::op_func(DataType const& rhs) const \
	{ \
		List<DataType> out = *this; \
		return out op_assign rhs; \
	}
	
	hm_define_list_datatype_op(+=, operator+)
	hm_define_list_datatype_op(-=, operator-)
	hm_define_list_datatype_op(*=, operator*)
	hm_define_list_datatype_op(/=, operator/)
	

	
#define hm_define_list_unary_op(op, op_func) \
	template <typename DataType> \
	List<DataType> List<DataType>::op_func() const \
	{ \
		List<DataType> out; \
		out.reserve(value.size()); \
		for (DataType const& x: value) \
		{ \
			out.value.push_back(op x); \
		} \
	}

	hm_define_list_unary_op(+, operator+)
	hm_define_list_unary_op(-, operator-)

	
	// Free operators with container left hand side
#define hm_define_datatype_list_op(op_func) \
template <typename DataType> \
List<DataType> op_func (DataType const& lhs, List<DataType> const& rhs) \
{ \
return op_func(rhs, lhs); \
}
	
	hm_define_datatype_list_op(operator+)
	hm_define_datatype_list_op(operator*)

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
	
}




