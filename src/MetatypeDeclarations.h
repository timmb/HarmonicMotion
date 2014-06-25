#pragma once
#include <QMetaType.h>
#include "Node.h"
#include "PatchCord.h"
#include "Inlet.h"
#include "Outlet.h"
#include "Common.h"
#include "nodes/NodeRenderer.h"
#include "Parameter.h"
#include "Utilities.h"

// These allow us to use these types in queued signal/slot connections

Q_DECLARE_METATYPE(std::shared_ptr<hm::Node>);
Q_DECLARE_METATYPE(std::shared_ptr<hm::PatchCord>);
Q_DECLARE_METATYPE(std::shared_ptr<hm::Outlet>);
Q_DECLARE_METATYPE(std::shared_ptr<hm::Inlet>);
Q_DECLARE_METATYPE(std::shared_ptr<hm::NodeRenderer>);
Q_DECLARE_METATYPE(std::shared_ptr<hm::BaseParameter>);

namespace hm
{
	void registerMetaTypes();
}