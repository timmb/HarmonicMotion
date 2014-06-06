//
//  Data.cpp
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 26/09/2013.
//
//

#include "Data.h"
#include <sstream>

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


Data::Data(Value const& x)
: mType(VALUE)
, data(x)
{}


Data::Data(Point3d const& x)
: mType(POINT3D)
, data(x)
{}

Data::Data(Skeleton3d const& x)
: mType(SKELETON3D)
, data(x)
{}

Data::Data(Scene3d const& x)
: mType(SCENE3D)
, data(x)
{}

BaseData* Data::asBaseData()
{
	switch (mType)
	{
		case UNDEFINED:
			return &boost::get<DataNull&>(data);
		case VALUE:
			return &boost::get<Value&>(data);
		case POINT3D:
			return &boost::get<Point3d&>(data);
		case SKELETON3D:
			return &boost::get<Skeleton3d&>(data);
		case SCENE3D:
			return &boost::get<Scene3d&>(data);
		default:
			assert(false);
			return nullptr;
	}
}


BaseData const* Data::asBaseData() const
{
	switch (mType)
	{
		case UNDEFINED:
			return &boost::get<DataNull const&>(data);;
		case VALUE:
			return &boost::get<Value const&>(data);
		case POINT3D:
			return &boost::get<Point3d const&>(data);
		case SKELETON3D:
			return &boost::get<Skeleton3d const&>(data);
		case SCENE3D:
			return &boost::get<Scene3d const&>(data);
		default:
			assert(false);
			return nullptr;
	}
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

bool Data::isValue() const
{
	return mType == VALUE;
}

Value const& Data::asValue() const
{
	assert(isValue());
	return boost::get<Value const&>(data);
}

Value& Data::asValue()
{
	assert(isValue());
	return boost::get<Value&>(data);
}

bool Data::isPoint3d() const
{
	return mType == POINT3D;
}

Point3d const& Data::asPoint3d() const
{
	assert(isPoint3d());
	return boost::get<Point3d const&>(data);
}

Point3d& Data::asPoint3d()
{
	assert(isPoint3d());
	return boost::get<Point3d&>(data);
}


bool Data::isSkeleton3d() const
{
	return mType == SKELETON3D;
}

Skeleton3d const& Data::asSkeleton3d() const
{
	assert(isSkeleton3d());
	return boost::get<Skeleton3d const&>(data);
}

Skeleton3d& Data::asSkeleton3d()
{
	assert(isSkeleton3d());
	return boost::get<Skeleton3d&>(data);
}


bool Data::isScene3d() const
{
	return mType == SCENE3D;
}

Scene3d const& Data::asScene3d() const
{
	assert(isScene3d());
	return boost::get<Scene3d const&>(data);
}

Scene3d& Data::asScene3d()
{
	assert(isScene3d());
	return boost::get<Scene3d&>(data);
}

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
	Data operator()(LhsType const& lhs, RhsType const& Rhs) \
	{ \
		assert(false && "Error: Cannot apply operation \""#OperationName"\" with objects of type "#LhsType" and "#RhsType"."); \
		return Data(); \
	}

	struct VisitorAdd : public static_visitor<Data>
	{
		BAD_OPERATION(addition, Value, Point3d)
		BAD_OPERATION(addition, Value, Skeleton3d)
		BAD_OPERATION(addition, Value, Scene3d)
		BAD_OPERATION(addition, Point3d, Value)
		BAD_OPERATION(addition, Skeleton3d, Value)
		BAD_OPERATION(addition, Scene3d, Value)

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
		
		template <typename T, typename U>
		Data operator()(T const& lhs, U const& rhs) const
		{
			return lhs + rhs;
		}
		

	};
	VisitorAdd visitorAdd;
	
	struct VisitorSub : public static_visitor<Data>
	{
		BAD_OPERATION(subtraction, Value, Point3d)
		BAD_OPERATION(subtraction, Value, Skeleton3d)
		BAD_OPERATION(subtraction, Value, Scene3d)
		BAD_OPERATION(subtraction, Point3d, Value)
		BAD_OPERATION(subtraction, Skeleton3d, Value)
		BAD_OPERATION(subtraction, Scene3d, Value)
		
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
		
		template <typename T, typename U>
		Data operator()(T const& lhs, U const& rhs) const
		{
			return lhs - rhs;
		}
	};
	VisitorSub visitorSub;
	
	
	struct VisitorMul : public static_visitor<Data>
	{
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
		Data operator()(T const& lhs, U const& rhs) const
		{
			return lhs * rhs;
		}
	};
	VisitorMul visitorMul;
	
	
	struct VisitorDiv : public static_visitor<Data>
	{
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
		Data operator()(T const& lhs, U const& rhs) const
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
			return +x;
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
	|| (t==IMAGE3D && u==IMAGE3D);
}

bool Data::canSubtract(Data const& rhs) const
{
	Type const& t = type();
	Type const& u = rhs.type();
	return (t==VALUE && u==VALUE)
	|| (t&VECTOR3D_TYPES && u&VECTOR3D_TYPES)
	|| (t==IMAGE3D && u==IMAGE3D);
}

bool Data::canMultiply(Data const& rhs) const
{
	Type const& t = type();
	Type const& u = rhs.type();
	return (t & VALUE && u & SCALABLE_TYPES)
	|| (t & SCALABLE_TYPES && u & VALUE)
	|| (t & VECTOR3D_TYPES && u & VECTOR3D_TYPES)
	|| (t & IMAGE3D && u & IMAGE3D);
}

bool Data::canDivide(Data const& rhs) const
{
	Type const& t = type();
	Type const& u = rhs.type();
	return (t & VALUE && u & SCALABLE_TYPES)
	|| (t & SCALABLE_TYPES && u & VALUE)
	|| (t & VECTOR3D_TYPES && u & VECTOR3D_TYPES)
	|| (t & IMAGE3D && u & IMAGE3D);

}







