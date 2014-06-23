//
//  DataNull.h
//  HarmonicMotion
//
//  Created by Tim Murray-Browne on 23/10/2013.
//
//

#pragma once

#include "BaseData.h"

namespace hm
{
	/// Class representing null data, i.e. something to use when we don't
	/// have any proper data
	class DataNull : public BaseData
	{
	public:
		DataNull(double timestamp_ = -1, int id = sDefaultId, SceneMetaPtr sceneMeta = SceneMeta::sDefaultSceneMeta);
		
		virtual std::ostream& printTo(std::ostream& out) const override;
		virtual Type type() const override { return UNDEFINED; }
	};
}