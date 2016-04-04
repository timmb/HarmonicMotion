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
#include "Functions.h"

using namespace hm;
using namespace std;



Data::Data()
: data(DataNull())
, mType(UNDEFINED)
{}


Data::Data(double x)
: data(Value(x))
, mType(VALUE)
{}

#define hm_data_define_constructor(Type) \
Data::Data(Type const& x) \
: data(x) \
, mType(getType<Type>()) \
{}

hm_data_define_constructor(DataNull)
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
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

// MARK: Operators

namespace
{
	using boost::static_visitor;
	
	struct VisitorAdd : public static_visitor<Data>
	{
		template <typename T>
		Data operator()(T const& lhs, DataNull const& rhs)
		{
			// will always fail.
//			assert(rhs.type() != UNDEFINED && "Error: Attempting to pass DataNull through addition operator.");
			return Data();
		}
		
		template <typename U>
		Data operator()(DataNull const& lhs, U const& rhs)
		{
//			assert(lhs.type() != UNDEFINED && "Attempting to pass DataNull through addition operator.");
			return Data();
		}
		
		Data operator()(DataNull const& lhs, DataNull const& rhs)
		{
			// will always fail.
//			assert(rhs.type() != UNDEFINED && "Attempting to pass DataNull through addition operator.");
			return Data();
		}
		
		template <typename T, typename U>
		typename enable_if<supports_addition<T, U>::value, Data>::type
		operator()(T const& lhs, U const& rhs) const
		{
			return lhs + rhs;
		}
		
		template <typename T, typename U>
		typename enable_if<!supports_addition<T,U>::value, Data>::type
		operator()(T const& lhs, U const& rhs) const
		{
//			BOOST_ASSERT_MSG(false, ("Error: Cannot add objects of type "+stringRepresentation<T>()+" and "+stringRepresentation<U>()+".").c_str());
			return Data();
		}
		

	};
	VisitorAdd visitorAdd;
	
	struct VisitorSub : public static_visitor<Data>
	{
		template <typename T>
		Data operator()(T const& lhs, DataNull const& rhs)
		{
			// will always fail.
//			assert(rhs.type() != UNDEFINED && "Error: Attempting to pass DataNull through subtraction operator.");
			return Data();
		}
		
		template <typename U>
		Data operator()(DataNull const& lhs, U const& rhs)
		{
//			assert(lhs.type() != UNDEFINED && "Attempting to pass DataNull through subtraction operator.");
			return Data();
		}
		
		Data operator()(DataNull const& lhs, DataNull const& rhs)
		{
			// will always fail.
//			assert(rhs.type() != UNDEFINED && "Attempting to pass DataNull through subtraction operator.");
			return Data();
		}
		
		// see comment in VisitorAdd
		template <typename T, typename U>
		typename enable_if<supports_addition<T, U>::value, Data>::type
		operator()(T const& lhs, U const& rhs) const
		{
			return lhs - rhs;
		}

		template <typename T, typename U>
		typename enable_if<!supports_addition<T,U>::value, Data>::type
		operator()(T const& lhs, U const& rhs) const
		{
//			BOOST_ASSERT_MSG(false, ("Error: Cannot subtract objects of type "+stringRepresentation<T>()+" and "+stringRepresentation<U>()+".").c_str());
			return Data();
		}
	};
	VisitorSub visitorSub;
	
	
	struct VisitorMul : public static_visitor<Data>
	{
		template <typename T>
		Data operator()(T const& lhs, DataNull const& rhs)
		{
			// will always fail.
//			assert(rhs.type() != UNDEFINED && "Attempting to pass DataNull through multiplication operator.");
			return Data();
		}
		
		template <typename U>
		Data operator()(DataNull const& lhs, U const& rhs)
		{
//			assert(lhs.type() != UNDEFINED && "Attempting to pass DataNull through multiplication operator.");
			return Data();
		}
		
		Data operator()(DataNull const& lhs, DataNull const& rhs)
		{
			// will always fail.
//			assert(rhs.type() != UNDEFINED && "Attempting to pass DataNull through multiplication operator.");
			return Data();
		}
		
		// see comment in VisitorAdd
		template <typename T, typename U>
		typename enable_if<supports_multiplication<T, U>::value, Data>::type
		operator()(T const& lhs, U const& rhs) const
		{
			return lhs * rhs;
		}

		template <typename T, typename U>
		typename enable_if<!supports_multiplication<T,U>::value, Data>::type
		operator()(T const& lhs, U const& rhs) const
		{
//			BOOST_ASSERT_MSG(false, ("Error: Cannot multiply objects of type "+stringRepresentation<T>()+" and "+stringRepresentation<U>()+".").c_str());
			return Data();
		}
	};
	VisitorMul visitorMul;
	
	
	struct VisitorDiv : public static_visitor<Data>
	{
		template <typename T>
		Data operator()(T const& lhs, DataNull const& rhs)
		{
			// will always fail.
//			assert(rhs.type() != UNDEFINED && "Attempting to pass DataNull through multiplication operator.");
			return Data();
		}
		
		template <typename U>
		Data operator()(DataNull const& lhs, U const& rhs)
		{
//			assert(lhs.type() != UNDEFINED && "Attempting to pass DataNull through multiplication operator.");
			return Data();
		}
		
		Data operator()(DataNull const& lhs, DataNull const& rhs)
		{
			// will always fail.
//			assert(rhs.type() != UNDEFINED && "Attempting to pass DataNull through multiplication operator.");
			return Data();
		}
		
		template <typename T, typename U>
		typename enable_if<supports_multiplication<T, U>::value, Data>::type
		operator()(T const& lhs, U const& rhs) const
		{
			return lhs / rhs;
		}
		
		template <typename T, typename U>
		typename enable_if<!supports_multiplication<T,U>::value, Data>::type
		operator()(T const& lhs, U const& rhs) const
		{
//			BOOST_ASSERT_MSG(false, ("Error: Cannot divide objects of type "+stringRepresentation<T>()+" and "+stringRepresentation<U>()+".").c_str());
			return Data();
		}
	};
	VisitorDiv visitorDiv;
	
	

	struct VisitorPos : public static_visitor<Data>
	{
		Data operator()(DataNull const& x)
		{
			// will always fail.
//			assert(x.type() != UNDEFINED && "Attempting to pass DataNull through unary positive operator.");
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
//			assert(x.type() != UNDEFINED && "Attempting to pass DataNull through unary negation operator.");
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


namespace
{
	struct SupportsAdditionVisitor : public boost::static_visitor<bool>
	{
		bool operator()(Image2d const& lhs, Image2d const& rhs) const
		{
			return lhs.value.type() == rhs.value.type() && lhs.value.size() == rhs.value.size();
		}

		template <typename T, typename U>
		bool operator()(T const& lhs, U const& rhs) const
		{
			return supports_addition<T, U>::value;
		}
	};
	SupportsAdditionVisitor supportsAdditionVisitor;
	
	struct SupportsMultiplicationVisitor : public boost::static_visitor<bool>
	{
		bool operator()(Image2d const& lhs, Image2d const& rhs) const
		{
			return lhs.value.channels() == rhs.value.channels() && lhs.value.size() == rhs.value.size();
		}

		template <typename T, typename U>
		bool operator()(T const& lhs, U const& rhs) const
		{
			return supports_multiplication<T, U>::value;
		}
	};
	SupportsMultiplicationVisitor supportsMultiplicationVisitor;
}


bool Data::canAdd(Data const& rhs) const
{
	return boost::apply_visitor(supportsAdditionVisitor, this->data, rhs.data);
}

bool Data::canSubtract(Data const& rhs) const
{
	return boost::apply_visitor(supportsAdditionVisitor, this->data, rhs.data);
}

bool Data::canMultiply(Data const& rhs) const
{
	return boost::apply_visitor(supportsMultiplicationVisitor, this->data, rhs.data);
}

bool Data::canDivide(Data const& rhs) const
{
	return boost::apply_visitor(supportsMultiplicationVisitor, this->data, rhs.data);
}


namespace
{
	Data const dataNull;
	struct AdditiveIdentityVisitor : public boost::static_visitor<Data const&>
	{
		Data const& operator()(DataNull const& x) const
		{
			return dataNull;
		}
		
		template <typename T>
		Data const& operator()(T const& x) const
		{
			static Data identity = additive_identity<T>::value;
			return identity;
		}
	};
	AdditiveIdentityVisitor additiveIdentityVisitor;
	
	struct MultiplicativeIdentityVisitor : public boost::static_visitor<Data const&>
	{
		Data const& operator()(DataNull const& x) const
		{
			return dataNull;
		}
		
		template <typename T>
		Data const& operator()(T const& x) const
		{
			static Data identity = multiplicative_identity<T>::value;
			return identity;
		}
	};
	MultiplicativeIdentityVisitor multiplicativeIdentityVisitor;
}

Data const& Data::additiveIdentity() const
{
	return boost::apply_visitor(additiveIdentityVisitor, data);
}

Data const& Data::multiplicativeIdentity() const
{
	return boost::apply_visitor(multiplicativeIdentityVisitor, data);
}


namespace
{
	struct MagnitudeVisitor : public boost::static_visitor<Data>
	{
		Data operator()(Scene3d const& x) const
		{
			return dataNull;
		}
		
		Data operator()(DataNull const& x) const
		{
			return dataNull;
		}
		
		template <typename T>
		Data operator()(T const& x) const
		{
			return magnitude(x);
		}
	};
	MagnitudeVisitor magnitudeVisitor;
}

Data Data::magnitude() const
{
	return boost::apply_visitor(magnitudeVisitor, data);
}


namespace
{
	struct CanMaximumVisitor : public boost::static_visitor<bool>
	{
		template <typename T>
		bool operator()(T const& x) const
		{
			return supports_maximum<T>::value;
		}

	};
	CanMaximumVisitor canMaximumVisitor;

	struct CanMinimumVisitor : public boost::static_visitor<bool>
	{
		template <typename T>
		bool operator()(T const& x) const
		{
			return supports_minimum<T>::value;
		}
	};
	CanMinimumVisitor canMinimumVisitor;
	
	struct MaximumVisitor : public boost::static_visitor<Data>
	{
		template <typename T>
		Data operator()(T const& lhs, T const& rhs) const
		{
			return Data(maximum(lhs, rhs));
		}
		
		Data operator()(DataNull const& lhs, DataNull const& rhs) const
		{
			return dataNull;
		}
		
		template <typename T, typename U>
		typename enable_if<!is_same<T,U>::value, Data>::type
		operator()(T const& lhs, U const& rhs) const
		{
			return dataNull;
		}
	};
	MaximumVisitor maximumVisitor;

	struct MinimumVisitor : public boost::static_visitor<Data>
	{
		template <typename T>
		Data operator()(T const& lhs, T const& rhs) const
		{
			return Data(minimum(lhs, rhs));
		}

		Data operator()(DataNull const& lhs, DataNull const& rhs) const
		{
			return dataNull;
		}

		template <typename T, typename U>
		typename enable_if<!is_same<T, U>::value, Data>::type
			operator()(T const& lhs, U const& rhs) const
		{
				return dataNull;
			}
	};
	MinimumVisitor minimumVisitor;

}

bool hm::canMaximum(Data const& lhs, Data const& rhs)
{
	return lhs.type()==rhs.type() && boost::apply_visitor(canMaximumVisitor, lhs.data);
}


Data hm::maximum(Data const& lhs, Data const& rhs)
{
	return lhs.type()==rhs.type()? boost::apply_visitor(maximumVisitor, lhs.data, rhs.data) : dataNull;
}

bool hm::canMinimum(Data const& lhs, Data const& rhs)
{
	return lhs.type() == rhs.type() && boost::apply_visitor(canMinimumVisitor, lhs.data);
}


Data hm::minimum(Data const& lhs, Data const& rhs)
{
	return lhs.type() == rhs.type() ? boost::apply_visitor(minimumVisitor, lhs.data, rhs.data) : dataNull;
}









