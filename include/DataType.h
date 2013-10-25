//
//  DataType.h
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
	class DataType
	{
	public:
		DataType(SceneMetaPtr sceneMeta_=nullptr) : sceneMeta(sceneMeta_) {}
		virtual ~DataType() {}
		/// Assumes we are already in a GL context
		virtual void draw() {}
		virtual std::string toString() const;
		virtual Type type() const = 0;
		
		/// Print to the stream and return it
		virtual std::ostream& printTo(std::ostream&) const = 0;
		
		/// TODO: Remove this function
		/// Some DataTypes have metadata attached describing the scene
		/// they belong in.
		virtual bool hasSceneMeta() const { return false; }
		/// This shall equal nullptr if and only if hasSceneMeta() is false
		SceneMetaPtr sceneMeta;
		/// Every time this DataType leaves a node, its name is added to
		/// the front of this list (up to a maximum number)
		std::deque<std::string> nodeHistory;
		static const int MAX_HISTORY_LENGTH = 100;
	};
	std::ostream& operator<<(std::ostream&, DataType const&);
}