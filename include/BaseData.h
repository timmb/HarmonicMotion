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
		Base1dData(double timestamp, SceneMetaPtr sceneMeta)
		: BaseData(timestamp, sceneMeta)
		{}
	};
	
	
	// Helper macro to declare basic operators on classes of the same type
	// use hm_data_define_self_operators_and_assigns(Type) to do everything
	
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

	// requires `value` to support the operators
#define hm_data_define_self_op_assign(Type, op_assign) \
	inline Type& Type:: op_assign (Type const& rhs) \
	{ \
		timestamp = max(timestamp, rhs.timestamp); \
		value.operator+=(rhs.value); \
		return *this; \
	}
	
#define hm_data_define_self_op(Type, op_func, op_assign) \
	inline Type Type:: op_func (Type const& rhs) const \
	{ \
		Type out = *this; \
		return out op_assign rhs; \
	}
	
#define hm_data_define_self_unary(Type, op_func) \
	inline Type Type:: op_func () const \
	{ \
		return Type(value.op_func(), timestamp, sceneMeta); \
	}
	
#define hm_data_define_self_operators_and_assigns(Type) \
	hm_data_define_self_op_assign(Type, operator+=) \
	hm_data_define_self_op_assign(Type, operator-=) \
	hm_data_define_self_op_assign(Type, operator*=) \
	hm_data_define_self_op_assign(Type, operator/=) \
	hm_data_define_self_op(Type, operator+, +=) \
	hm_data_define_self_op(Type, operator-, -=) \
	hm_data_define_self_op(Type, operator*, *=) \
	hm_data_define_self_op(Type, operator/, /=) \
	hm_data_define_self_unary(Type, operator+) \
	hm_data_define_self_unary(Type, operator-)
	
	// Macros to help define operations of a type against a scalar
	// type (Value, double or float)
	// Use hm_data_define_scalar_operators_and_assigns(Type)
	// to do everything
	
#define hm_data_define_single_scalar_op_assign(Type, op_assign, Scalar) \
	inline Type& Type:: op_assign (Scalar const& rhs) \
	{ \
		value.op_assign(rhs); \
		return *this; \
	}
	
#define hm_data_define_scalar_op_assign(Type, op_assign) \
	hm_data_define_single_scalar_op_assign(Type, op_assign, Value) \
	hm_data_define_single_scalar_op_assign(Type, op_assign, float) \
	hm_data_define_single_scalar_op_assign(Type, op_assign, double) \

	
#define hm_data_define_single_scalar_op(Type, op, op_assign, Scalar) \
	inline Type Type:: op (Scalar const& rhs) const \
	{ \
		return Type(*this).op_assign(rhs); \
	}

#define hm_data_define_scalar_op(Type, op, op_assign) \
hm_data_define_single_scalar_op(Type, op, op_assign, Value) \
hm_data_define_scalar_op(Type, op, op_assign, float) \
hm_data_define_scalar_op(Type, op, op_assign, double) \

#define hm_data_define_scalar_op_and_assign(Type, op, op_assign) \
	hm_data_define_scalar_op_assign(Type, op_assign) \
	hm_data_define_scalar_op(Type, op, op_assign) \

#define hm_data_define_scalar_operators_and_assigns(Type) \
hm_data_define_scalar_op_and_assign(Type, operator+, operator+=) \
hm_data_define_scalar_op_and_assign(Type, operator-, operator-=) \
hm_data_define_scalar_op_and_assign(Type, operator*, operator*=) \
hm_data_define_scalar_op_and_assign(Type, operator/, operator/=) \

	inline Type op (Scalar const& lhs, Type rhs)
	{
		rhs.value = op(lhs, rhs.value);
		return rhs;
	}
	
}











