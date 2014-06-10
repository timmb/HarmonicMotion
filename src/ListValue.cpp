#include "ListValue.h"

using namespace hm;
using namespace std;

ListValue::ListValue(double timestamp, SceneMetaPtr sceneMeta)
: List<Value>(timestamp, sceneMeta)
{}

ListValue::ListValue(vector<Value> values, double timestamp, SceneMetaPtr sceneMeta)
: List<Value>(timestamp, sceneMeta)
, value(values)
{}


#define hm_define_value_assign(op_func) \
ListValue& ListValue:: op_func (Value const& rhs) \
{ \
for (Value& x: value) \
{ \
x.op_func(rhs.value); \
} \
return *this; \
}

hm_define_value_assign(operator*=)
hm_define_value_assign(operator/=)



#define hm_define_value_op(op_assign, op_func) \
ListValue ListValue:: op_func (Value const& rhs) const\
{ \
ListValue out = *this; \
return out op_assign rhs; \
}

hm_define_value_op(*=, operator*)
hm_define_value_op(/=, operator/)

