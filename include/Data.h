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
#include <boost/variant.hpp>

namespace hm
{
	/// Variant container for all data types
	class Data
	{
	public:
		/// Creates a null (i.e. undefined) Data object with timestamp
		/// -42.
		Data();
		/// Creates a Data object with a default initialized Type and
		/// given timestamp
		Data(Type type, double timestamp);
		Data(Value& value, double timestamp);
		Data(Point3d& value, double timestamp);
		Data(Skeleton3d& value, double timestamp);
		Data(Scene3d& value, double timestamp);

		Type type() const { return mType; }
		double timestamp() const { return mTimestamp; }
		
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
		
	private:
		typedef boost::variant<Value, Point3d, Skeleton3d, Scene3d> Variant;
		Type mType;
		Variant mData;
		double mTimestamp;
		
		friend std::ostream& operator<<(std::ostream&, Data&);
	};
	
	inline std::ostream& operator<<(std::ostream& out, Data& rhs)
	{
		return out << rhs.toString();
	}
}