//
//  Data.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 26/09/2013.
//
//

#include "Data.h"
#include <sstream>
#include <type_traits>
#include <utility>

using namespace hm;
using namespace std;



Data::Data()
: mType(UNDEFINED)
, data(DataNull())
{}


Data::Data(double x)
: mType(VALUE)
, data(Value(x))
{}

#define hm_data_define_constructor(Type) \
Data::Data(Type const& x) \
: mType(getType<Type>()) \
, data(x) \
{}

hm_data_define_constructor(Value)
hm_data_define_constructor(Point2d)
hm_data_define_constructor(Point3d)
hm_data_define_constructor(Skeleton3d)
hm_data_define_constructor(Scene3d)
hm_data_define_constructor(ListValue)
hm_data_define_constructor(ListPoint2d)
hm_data_define_constructor(ListPoint3d)
hm_data_define_constructor(Image2d)


namespace {
	struct GetBaseDataVisitor : public boost::static_visitor<BaseData*>
	{
		BaseData* operator()(BaseData & x) const
		{
			return &x;
		}
	};
	GetBaseDataVisitor getBaseDataVisitor;
	
	struct GetConstBaseDataVisitor : public boost::static_visitor<BaseData const*>
	{
		BaseData const* operator()(BaseData const& x) const
		{
			return &x;
		}
	};
	GetConstBaseDataVisitor getConstBaseDataVisitor;

}



BaseData* Data::asBaseData()
{
	return boost::apply_visitor(getBaseDataVisitor, data);
}


BaseData const* Data::asBaseData() const
{
	return boost::apply_visitor(getConstBaseDataVisitor, data);
}

bool Data::isNull() const
{
	return mType == UNDEFINED;
}

DataNull& Data::asNull()
{
	return boost::get<DataNull&>(data);
}

DataNull const& Data::asNull() const
{
	return boost::get<DataNull const&>(data);
}

#define hm_data_is_and_as_functions(Type) \
bool Data::is##Type() const \
{ \
	return mType == getType<Type>(); \
} \
 \
Type const& Data::as##Type() const \
{ \
	assert(is##Type()); \
	return boost::get<Type const&>(data); \
} \
 \
Type& Data::as##Type() \
{ \
	assert(is##Type()); \
	return boost::get<Type&>(data); \
}

hm_data_is_and_as_functions(Value)
hm_data_is_and_as_functions(Point2d)
hm_data_is_and_as_functions(Point3d)
hm_data_is_and_as_functions(Skeleton3d)
hm_data_is_and_as_functions(Scene3d)
hm_data_is_and_as_functions(Image2d)
hm_data_is_and_as_functions(ListValue)
hm_data_is_and_as_functions(ListPoint2d)
hm_data_is_and_as_functions(ListPoint3d)

namespace hm {
	std::ostream& operator<<(std::ostream& out, Data const& rhs)
	{
		return out << *rhs.asBaseData();
	}
}

std::string Data::toString() const
{
	return (std::stringstream() << *this).str();
}

// MARK: Operators

namespace
{
	using boost::static_visitor;
	
#define BAD_OPERATION(OperationName, LhsType, RhsType) \
	Data operator()(LhsType const& lhs, RhsType const& Rhs) const \
	{ \
		assert(false && "Error: Cannot apply operation \""#OperationName"\" with objects of type "#LhsType" and "#RhsType"."); \
		return Data(); \
	}
	
	struct VisitorAdd : public static_visitor<Data>
	{
		BAD_OPERATION(addition, Value, Base2dData)
		BAD_OPERATION(addition, Base1dData, Base3dData)
		BAD_OPERATION(addition, Base2dData, Base1dData)
		BAD_OPERATION(addition, Base2dData, Base3dData)
		BAD_OPERATION(addition, Base3dData, Base1dData)
		BAD_OPERATION(addition, Base3dData, Base2dData)
		BAD_OPERATION(addition, Image2d, Base2dData)
		BAD_OPERATION(addition, Image2d, Base3dData)
		BAD_OPERATION(addition, Base2dData, Image2d)
		BAD_OPERATION(addition, Base3dData, Image2d)
		BAD_OPERATION(addition, Skeleton3d, ListPoint3d)
		BAD_OPERATION(addition, ListPoint3d, Skeleton3d)

		template <typename T>
		Data operator()(T const& lhs, DataNull const& rhs)
		{
			// will always fail.
			assert(rhs.type() != UNDEFINED && "Error: Attempting to pass DataNull through addition operator.");
			return Data();
		}
		
		template <typename U>
		Data operator()(DataNull const& lhs, U const& rhs)
		{
			assert(lhs.type() != UNDEFINED && "Attempting to pass DataNull through addition operator.");
			return Data();
		}
		
		Data operator()(DataNull const& lhs, DataNull const& rhs)
		{
			// will always fail.
			assert(rhs.type() != UNDEFINED && "Attempting to pass DataNull through addition operator.");
			return Data();
		}
		
		// We use decltype(lhs+rhs) with auto here instead of an explicit
		// return type to prevent this template function from being selected
		// as a preferred candidate for the BAD_OPERATION types listed above.
		template <typename T, typename U>
		typename enable_if<supports_addition<T, U>::value, Data>::type
		operator()(T const& lhs, U const& rhs) const
		{
			return lhs + rhs;
		}
		

	};
	VisitorAdd visitorAdd;
	
	struct VisitorSub : public static_visitor<Data>
	{
		BAD_OPERATION(subtraction, Base1dData, Base2dData)
		BAD_OPERATION(subtraction, Base1dData, Base3dData)
		BAD_OPERATION(subtraction, Base2dData, Base1dData)
		BAD_OPERATION(subtraction, Base2dData, Base3dData)
		BAD_OPERATION(subtraction, Base3dData, Base1dData)
		BAD_OPERATION(subtraction, Base3dData, Base2dData)
		BAD_OPERATION(subtraction, Image2d, Base2dData)
		BAD_OPERATION(subtraction, Image2d, Base3dData)
		BAD_OPERATION(subtraction, Base2dData, Image2d)
		BAD_OPERATION(subtraction, Base3dData, Image2d)
		BAD_OPERATION(subtraction, Skeleton3d, ListPoint3d)
		BAD_OPERATION(subtraction, ListPoint3d, Skeleton3d)

		
		template <typename T>
		Data operator()(T const& lhs, DataNull const& rhs)
		{
			// will always fail.
			assert(rhs.type() != UNDEFINED && "Error: Attempting to pass DataNull through subtraction operator.");
			return Data();
		}
		
		template <typename U>
		Data operator()(DataNull const& lhs, U const& rhs)
		{
			assert(lhs.type() != UNDEFINED && "Attempting to pass DataNull through subtraction operator.");
			return Data();
		}
		
		Data operator()(DataNull const& lhs, DataNull const& rhs)
		{
			// will always fail.
			assert(rhs.type() != UNDEFINED && "Attempting to pass DataNull through subtraction operator.");
			return Data();
		}
		
		// see comment in VisitorAdd
		template <typename T, typename U>
		typename enable_if<supports_addition<T, U>::value, Data>::type
		operator()(T const& lhs, U const& rhs) const
		{
			return lhs - rhs;
		}
	};
	VisitorSub visitorSub;
	
	
	struct VisitorMul : public static_visitor<Data>
	{
		BAD_OPERATION(multiplication, Base2dData, Image2d)
		BAD_OPERATION(multiplication, Base2dData, Base3dData)
		BAD_OPERATION(multiplication, Base3dData, Image2d)
		BAD_OPERATION(multiplication, Base3dData, Base2dData)
		BAD_OPERATION(multiplication, Skeleton3d, ListPoint3d)
		BAD_OPERATION(multiplication, ListPoint3d, Skeleton3d)

		template <typename T>
		Data operator()(T const& lhs, DataNull const& rhs)
		{
			// will always fail.
			assert(rhs.type() != UNDEFINED && "Attempting to pass DataNull through multiplication operator.");
			return Data();
		}
		
		template <typename U>
		Data operator()(DataNull const& lhs, U const& rhs)
		{
			assert(lhs.type() != UNDEFINED && "Attempting to pass DataNull through multiplication operator.");
			return Data();
		}
		
		Data operator()(DataNull const& lhs, DataNull const& rhs)
		{
			// will always fail.
			assert(rhs.type() != UNDEFINED && "Attempting to pass DataNull through multiplication operator.");
			return Data();
		}
		
		// see comment in VisitorAdd
		template <typename T, typename U>
		typename enable_if<supports_multiplication<T, U>::value, Data>::type
		operator()(T const& lhs, U const& rhs) const
		{
			return lhs * rhs;
		}
	};
	VisitorMul visitorMul;
	
	
	struct VisitorDiv : public static_visitor<Data>
	{
		BAD_OPERATION(division, Base2dData, Image2d)
		BAD_OPERATION(division, Base2dData, Base3dData)
		BAD_OPERATION(division, Base3dData, Image2d)
		BAD_OPERATION(division, Base3dData, Base2dData)
		BAD_OPERATION(division, Skeleton3d, ListPoint3d)
		BAD_OPERATION(division, ListPoint3d, Skeleton3d)

		template <typename T>
		Data operator()(T const& lhs, DataNull const& rhs)
		{
			// will always fail.
			assert(rhs.type() != UNDEFINED && "Attempting to pass DataNull through multiplication operator.");
			return Data();
		}
		
		template <typename U>
		Data operator()(DataNull const& lhs, U const& rhs)
		{
			assert(lhs.type() != UNDEFINED && "Attempting to pass DataNull through multiplication operator.");
			return Data();
		}
		
		Data operator()(DataNull const& lhs, DataNull const& rhs)
		{
			// will always fail.
			assert(rhs.type() != UNDEFINED && "Attempting to pass DataNull through multiplication operator.");
			return Data();
		}
		
		template <typename T, typename U>
		typename enable_if<supports_multiplication<T, U>::value, Data>::type
		operator()(T const& lhs, U const& rhs) const
		{
			return lhs / rhs;
		}
	};
	VisitorDiv visitorDiv;
	
	

	struct VisitorPos : public static_visitor<Data>
	{
		Data operator()(DataNull const& x)
		{
			// will always fail.
			assert(x.type() != UNDEFINED && "Attempting to pass DataNull through unary positive operator.");
			return Data();
		}
		
		template <typename T>
		Data operator()(T const& x) const
		{
			return x;
		}
	};
	VisitorPos visitorPos;

	
	struct VisitorNeg : public static_visitor<Data>
	{
		Data operator()(DataNull const& x)
		{
			// will always fail.
			assert(x.type() != UNDEFINED && "Attempting to pass DataNull through unary negation operator.");
			return Data();
		}
		
		template <typename T>
		Data operator()(T const& x) const
		{
			return -x;
		}
	};
	VisitorNeg visitorNeg;


}


Data Data::operator+(Data const& rhs) const
{
	return boost::apply_visitor(visitorAdd, this->data, rhs.data);
}

Data Data::operator-(Data const& rhs) const
{
	return boost::apply_visitor(visitorSub, this->data, rhs.data);
}

Data Data::operator*(Data const& rhs) const
{
	return boost::apply_visitor(visitorMul, this->data, rhs.data);
}

Data Data::operator/(Data const& rhs) const
{
	return boost::apply_visitor(visitorDiv, this->data, rhs.data);
}

Data Data::operator-() const
{
	return boost::apply_visitor(visitorNeg, this->data);
}

Data Data::operator+() const
{
	return boost::apply_visitor(visitorPos, this->data);
}

bool Data::canAdd(Data const& rhs) const
{
	Type const& t = type();
	Type const& u = rhs.type();
	return (t==VALUE && u==VALUE)
	|| (t&VECTOR3D_TYPES && u&VECTOR3D_TYPES)
	|| (t==IMAGE2D && u==IMAGE2D)
	|| (t==IMAGE2D && u==VALUE)
	|| (t==VALUE && u==IMAGE2D);
}

bool Data::canSubtract(Data const& rhs) const
{
	Type const& t = type();
	Type const& u = rhs.type();
	return (t==VALUE && u==VALUE)
	|| (t&VECTOR3D_TYPES && u&VECTOR3D_TYPES)
	|| (t==IMAGE2D && u==IMAGE2D)
	|| (t==IMAGE2D && u==VALUE)
	|| (t==VALUE && u==IMAGE2D);
}

bool Data::canMultiply(Data const& rhs) const
{
	Type const& t = type();
	Type const& u = rhs.type();
	return (t & VALUE && u & SCALABLE_TYPES)
	|| (t & SCALABLE_TYPES && u & VALUE)
	|| (t & VECTOR3D_TYPES && u & VECTOR3D_TYPES)
	|| (t==IMAGE2D && u==IMAGE2D)
	|| (t==IMAGE2D && u==VALUE)
	|| (t==VALUE && u==IMAGE2D);
}

bool Data::canDivide(Data const& rhs) const
{
	Type const& t = type();
	Type const& u = rhs.type();
	return (t & VALUE && u & SCALABLE_TYPES)
	|| (t & SCALABLE_TYPES && u & VALUE)
	|| (t & VECTOR3D_TYPES && u & VECTOR3D_TYPES)
	|| (t==IMAGE2D && u==IMAGE2D)
	|| (t==IMAGE2D && u==VALUE)
	|| (t==VALUE && u==IMAGE2D);

}







