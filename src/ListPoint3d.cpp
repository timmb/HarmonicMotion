#include "ListPoint3d.h"
#include "Value.h"

using namespace hm;
using namespace std;

//ListPoint3d::ListPoint3d(double timestamp, SceneMetaPtr sceneMeta)
//: List<Point3d>(timestamp, sceneMeta)
//{}
//
//ListPoint3d::ListPoint3d(vector<Point3d> values, double timestamp, SceneMetaPtr sceneMeta)
//: List<Point3d>(timestamp, sceneMeta)
//, value(values)
//{}


//#define hm_define_value_assign(op_func) \
//ListPoint3d& ListPoint3d:: op_func (Value const& rhs) \
//{ \
//	for (Point3d& x: value) \
//	{ \
//		x.op_func(rhs.value); \
//	} \
//	return *this; \
//}
//
//hm_define_value_assign(operator*=)
//hm_define_value_assign(operator/=)
//
//
//
//#define hm_define_value_op(op_assign, op_func) \
//ListPoint3d ListPoint3d:: op_func (Value const& rhs) const\
//{ \
//	ListPoint3d out = *this; \
//	return out op_assign rhs; \
//}
//
//hm_define_value_op(*=, operator*)
//hm_define_value_op(/=, operator/)
//
