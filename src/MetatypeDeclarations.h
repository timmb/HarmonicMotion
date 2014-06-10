#pragma once
#include "Node.h"
#include "PatchCord.h"
#include "Inlet.h"
#include "Outlet.h"
#include "Common.h"
#include "nodes/NodeRenderer.h"

Q_DECLARE_METATYPE(std::shared_ptr<hm::Node>);
Q_DECLARE_METATYPE(std::shared_ptr<hm::PatchCord>);
Q_DECLARE_METATYPE(std::shared_ptr<hm::Outlet>);
Q_DECLARE_METATYPE(std::shared_ptr<hm::Inlet>);
Q_DECLARE_METATYPE(std::shared_ptr<hm::NodeRenderer>);