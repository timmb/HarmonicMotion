//
//  BaseData.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 22/10/2013.
//
//

#pragma once
#include <string>
#include <ostream>
#include "Type.h"
#include "SceneMeta.h"
#include "Common.h"
#include <deque>
#include "TypeTraits.h"

namespace hm
{
	/// Base class for different Types
	class BaseData
	{
	public:
		BaseData(double timestamp_, SceneMetaPtr sceneMeta_);
		virtual ~BaseData() {}
		/// Assumes we are already in a GL context
		virtual void draw() {}
		virtual std::string toString() const;
		virtual Type type() const = 0;
		
		/// Print to the stream and return it
		virtual std::ostream& printTo(std::ostream&) const = 0;
		
		/// The latest time of entry associated with this data
		double timestamp;
		/// Information about the scene associated with this data
		SceneMetaPtr sceneMeta;
		/// Every time this DataType leaves a node, its name is added to
		/// the front of this list (up to a maximum number)
		std::deque<std::string> nodeHistory;
		static const int MAX_HISTORY_LENGTH = 100;
	};
	
	std::ostream& operator<<(std::ostream&, BaseData const&);

	// At this level, we distinguish between categories: 3D vector types, Scalars,
	// and Image2D types, etc. with the requirement that classes in the same
	// category support basic arithmetic operations on each other: +-*/
	
	/// Empty base classes to distinguish 3D Vector types (Point3d, Skeleton3d, Scene3d)
	class Base3dData : public BaseData
	{
	public:
		Base3dData(double timestamp, SceneMetaPtr sceneMeta)
		: BaseData(timestamp, sceneMeta)
		{}
	};
	
	class Base2dData : public BaseData
	{
	public:
		Base2dData(double timestamp, SceneMetaPtr sceneMeta)
		: BaseData(timestamp, sceneMeta)
		{}
	};
	
	class Base1dData : public BaseData
	{
	public:
		/// The presence of this constant indicates that this class supports
		/// addition/subtraction with scalars (i.e. other Base1dData)
		std::true_type supports_scalar_addition;

		Base1dData(double timestamp, SceneMetaPtr sceneMeta)
		: BaseData(timestamp, sceneMeta)
		{}
	};
	
	
	// Macros to declare basic operators (*,/,+,-,*=,/=,+=,-=)
	// on classes of the same type
	// use hm_data_define_self_operators_and_assigns(Type) to do everything
	// requires `value` member of Type to support the operators
	
	
//#define hm_data_declare_self_operators_and_assigns(Type) \
//		Type& operator+=(Type const& rhs); \
//		Type& operator-=(Type const& rhs); \
//		Type& operator*=(Type const& rhs); \
//		Type& operator/=(Type const& rhs); \
//		Type operator+(Type const& rhs) const; \
//		Type operator-(Type const& rhs) const; \
//		Type operator*(Type const& rhs) const; \
//		Type operator/(Type const& rhs) const; \
//		Type operator+() const; \
//		Type operator-() const;

	// Helper:
#define _hm_data_define_self_op_assign(Type, op_assign) \
	inline Type& op_assign (Type const& rhs) \
	{ \
		timestamp = std::max<double>(timestamp, rhs.timestamp); \
		value.op_assign(rhs.value); \
		return *this; \
	}
	
	// Helper:
#define _hm_data_define_self_op(Type, op_func, op_assign) \
	inline Type op_func (Type const& rhs) const \
	{ \
		Type out = *this; \
		return out.op_assign(rhs); \
	}
	
	// Helper:
#define _hm_data_define_self_unary(Type, op_func, op) \
	inline Type op_func () const \
	{ \
		return Type(op value, timestamp, sceneMeta); \
	}
	
#define hm_data_define_self_operators_and_assigns(Type) \
	_hm_data_define_self_op_assign(Type, operator+=) \
	_hm_data_define_self_op_assign(Type, operator-=) \
	_hm_data_define_self_op_assign(Type, operator*=) \
	_hm_data_define_self_op_assign(Type, operator/=) \
	_hm_data_define_self_op(Type, operator+, operator+=) \
	_hm_data_define_self_op(Type, operator-, operator-=) \
	_hm_data_define_self_op(Type, operator*, operator*=) \
	_hm_data_define_self_op(Type, operator/, operator/=) \
	/*hm_data_define_self_unary(Type, operator+, +)*/ \
	_hm_data_define_self_unary(Type, operator-, -)
	
	// Macros to help define operations of a type against a scalar
	// type (Value, double or float)
	// Use hm_data_define_scalar_operators_and_assigns(Type)
	// to do everything (+-*/ and assigns) or
	// hm_data_define_scalar_mult_operators_and_assigns(Type)
	// to just do */ (and *=, /=)

	// Helper:
#define _hm_data_define_single_scalar_op_assign(Type, op_assign, Scalar) \
	inline Type& operator op_assign (Scalar rhs) \
	{ \
		value op_assign rhs; \
		return *this; \
	}
	
	// Helper:
#define _hm_data_define_scalar_op_assign(Type, op_assign) \
	_hm_data_define_single_scalar_op_assign(Type, op_assign, float) \
	_hm_data_define_single_scalar_op_assign(Type, op_assign, double) \
	inline Type& operator op_assign (Value const& rhs) \
	{ \
		value op_assign rhs.value; \
		timestamp = std::max(timestamp, rhs.timestamp); \
		return *this; \
	}

	
	// Helper:
#define _hm_data_define_single_scalar_op(Type, op, op_assign, Scalar) \
	inline Type operator op (Scalar rhs) const \
	{ \
		return Type(*this) op_assign rhs; \
	}

	// Helper:
#define _hm_data_define_scalar_op(Type, op, op_assign) \
_hm_data_define_single_scalar_op(Type, op, op_assign, float) \
_hm_data_define_single_scalar_op(Type, op, op_assign, double) \
inline Type operator op (Value const& rhs) const \
{ \
	Type out(*this); \
	out op_assign rhs; \
	out.timestamp = std::max(timestamp, rhs.timestamp); \
	out.sceneMeta = choose(sceneMeta, rhs.sceneMeta); \
	return out; \
}

	// Helper:
#define _hm_data_define_scalar_op_and_assign(Type, op, op_assign) \
	_hm_data_define_scalar_op_assign(Type, op_assign) \
	_hm_data_define_scalar_op(Type, op, op_assign)

	// Member operators against scalars: just * and /
#define hm_data_define_scalar_mult_operators_and_assigns(Type) \
_hm_data_define_scalar_op_and_assign(Type, *, *=) \
_hm_data_define_scalar_op_and_assign(Type, /, /=)

	// Member operators against scalars: * / + -
#define hm_data_define_scalar_operators_and_assigns(Type) \
_hm_data_define_scalar_op_and_assign(Type, +, +=) \
_hm_data_define_scalar_op_and_assign(Type, -, -=) \
_hm_data_define_scalar_op_and_assign(Type, *, *=) \
_hm_data_define_scalar_op_and_assign(Type, /, /=)

	
	// Free scalar operators: e.g. operator+(Value lhs, Type rhs)
	// Use hm_data_define_free_scalar_operators(Type) to define them all
	// (+-*/) or

	// Helper:
#define _hm_data_define_free_single_scalar_op(Type, op, Scalar) \
	inline Type operator op (Scalar lhs, Type rhs) \
	{ \
		rhs.value = lhs op rhs.value; \
		return rhs; \
	}
	
	// Helper:
#define _hm_data_define_free_scalar_op(Type, op) \
	_hm_data_define_free_single_scalar_op(Type, op, float) \
	_hm_data_define_free_single_scalar_op(Type, op, double) \
	inline Type operator op (Value const& lhs, Type rhs) \
	{ \
		rhs.value = lhs.value op rhs.value; \
		rhs.timestamp = std::max(lhs.timestamp, rhs.timestamp); \
		return rhs; \
	}
	
#define hm_data_define_free_mult_scalar_operators(Type) \
	_hm_data_define_free_scalar_op(Type, *) \
	_hm_data_define_free_scalar_op(Type, /)

#define hm_data_define_free_scalar_operators(Type) \
	_hm_data_define_free_scalar_op(Type, +) \
	_hm_data_define_free_scalar_op(Type, -) \
	_hm_data_define_free_scalar_op(Type, *) \
	_hm_data_define_free_scalar_op(Type, /)
	
	
	

	// Equality and inequality operators. Checks that value, timestamp and
	// sceneMeta are equal. Use this in the class body.
	
	// Helper:
#define _hm_data_define_equality_op(Type, op) \
	bool operator op(Type const& rhs) const \
	{ \
		return BaseData::timestamp op rhs.timestamp && value op rhs.value && BaseData::sceneMeta op rhs.sceneMeta; \
	}
	
#define hm_data_define_equality_ops(Type) \
	_hm_data_define_equality_op(Type, ==) \
	_hm_data_define_equality_op(Type, !=)
	
}











