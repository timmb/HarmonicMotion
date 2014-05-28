//
//  Data.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 26/09/2013.
//
//

#pragma once
#include "Common.h"
#include "Skeleton3d.h"
#include "Scene3d.h"
#include "Point3d.h"
#include "Value.h"
#include "DataNull.h"
#include <boost/variant.hpp>

namespace hm
{
	/// Variant container for all DataTypes
	class Data
	{
	public:
		/// Creates a null (i.e. undefined) Data object with timestamp
		/// -42.
		Data();
		/// Creates a Data object with a default initialized Type and
		/// given timestamp)
//		Data(Type type, double timestamp);
		Data(Value const& value, double timestamp);
		Data(Point3d const& value, double timestamp);
		Data(Skeleton3d const& value, double timestamp);
		Data(Scene3d const& value, double timestamp);

		Type type() const { return mType; }
		double timestamp() const { return mTimestamp; }
		
		/// True if this object was constructed using the
		/// default constructor and has no type or data.
		bool isNull() const;
		DataNull& asNull();
		DataNull const& asNull() const;
		
		/// \return A pointer to the contained data or nullptr
		/// if isNull(). The pointer will be invalid once this object
		/// is destroyed
		DataType* asDataType();
		DataType const* asDataType() const;
		
		bool isValue() const;
		Value const& asValue() const;
		Value& asValue();
		
		bool isPoint3d() const;
		Point3d const& asPoint3d() const;
		Point3d& asPoint3d();
		
		bool isSkeleton3d() const;
		Skeleton3d const& asSkeleton3d() const;
		Skeleton3d& asSkeleton3d();

		bool isScene3d() const;
		Scene3d const& asScene3d() const;
		Scene3d& asScene3d();
		
		std::string toString() const;
		
		// Mathematical operators. See individual type definitions for notes
		// on the effect of operators on pairs of distinct types
		Data operator+(Data const& rhs);
		Data operator-(Data const& rhs);
		Data operator*(Data const& rhs);
		Data operator/(Data const& rhs);

		
	private:
		typedef boost::variant<DataNull, Value, Point3d, Skeleton3d, Scene3d> Variant;
//		static Variant createVariant(Type type);
		Type mType;
		Variant mData;
		double mTimestamp;
	};
	
	std::ostream& operator<<(std::ostream& out, Data const& rhs);
}

