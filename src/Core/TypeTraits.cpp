#include "TypeTraits.h"
#include "Types.h"

namespace hm
{
	// define corresponding static variable
#undef hm_declare_additive_identity
#define hm_declare_additive_identity(Type, identity, IdentityType) \
	IdentityType const additive_identity<Type>::value = identity;
	
#undef hm_declare_multiplicative_identity
#define hm_declare_multiplicative_identity(Type, identity, IdentityType) \
	IdentityType const multiplicative_identity<Type>::value = identity;
	
	hm_declare_additive_identity(float, 0.f, float)
	hm_declare_additive_identity(double, 0., double)
	hm_declare_additive_identity(Value, 0., double)
	hm_declare_additive_identity(Point2d, Point2d(), Point2d)
	hm_declare_additive_identity(Point3d, Point3d(), Point3d)
	hm_declare_additive_identity(Skeleton3d, Skeleton3d(), Skeleton3d)
	hm_declare_additive_identity(Scene3d, Scene3d(), Scene3d)
	hm_declare_additive_identity(Image2d, 0, int)
	hm_declare_additive_identity(ListValue, ListValue(), ListValue)
	hm_declare_additive_identity(ListPoint2d, ListPoint2d(), ListPoint2d)
	hm_declare_additive_identity(ListPoint3d, ListPoint3d(), ListPoint3d)
	
	hm_declare_multiplicative_identity(float, 1.f, float)
	hm_declare_multiplicative_identity(double, 1., double)
	hm_declare_multiplicative_identity(Value, 1., double)
	hm_declare_multiplicative_identity(Point2d, Point2d(ci::Vec2f(1,1)), Point2d)
	hm_declare_multiplicative_identity(Point3d, Point3d(ci::Vec3f(1,1,1)), Point3d)
	hm_declare_multiplicative_identity(Skeleton3d, Skeleton3d(), Skeleton3d)
	hm_declare_multiplicative_identity(Scene3d, Scene3d(), Scene3d)
	hm_declare_multiplicative_identity(Image2d, 0, int)
	hm_declare_multiplicative_identity(ListValue, ListValue(), ListValue)
	hm_declare_multiplicative_identity(ListPoint2d, ListPoint2d(), ListPoint2d)
	hm_declare_multiplicative_identity(ListPoint3d, ListPoint3d(), ListPoint3d)
	
}