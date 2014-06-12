#pragma once

#include "List.h"
#include "Value.h"

namespace hm
{
	
	typedef List<Value> ListValue;
//	/// Class representing a list of Values
//	class ListValue : public List<Value>
//	{
//	public:
//		typedef List<Value> BaseType;
//		
//		explicit
//		ListValue(double timestamp = 0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
//		ListValue(std::vector<Value> points, double timestamp=0., SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
//		
//		std::vector<Value> value;
//		
//		virtual Type type() const override { return LIST_VALUE; }
//	
////	ListValue& operator*=(Value const& rhs);
////	ListValue& operator/=(Value const& rhs);
////	ListValue operator*(Value const& rhs) const;
////	ListValue operator/(Value const& rhs) const;
//			
//};

}