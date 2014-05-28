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
	
	/// Empty base classes to distinguish 3D Vector types (Point3d, Skeleton3d, Scene3d)
	class Base3dData : public BaseData
	{
	public:
		Base3dData(double timestamp, SceneMetaPtr sceneMeta)
		: BaseData(timestamp, sceneMeta)
		{}
	};
	
	std::ostream& operator<<(std::ostream&, BaseData const&);
}