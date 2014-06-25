#include "MetaTypeDeclarations.h"

namespace hm
{
	void registerMetaTypes()
	{
		static bool isRegistered = false;
		if (isRegistered)
		{
			return;
		}
		qRegisterMetaType<std::shared_ptr<hm::Node>>();
		qRegisterMetaType<std::shared_ptr<hm::PatchCord>>();
		qRegisterMetaType<std::shared_ptr<hm::Outlet>>();
		qRegisterMetaType<std::shared_ptr<hm::Inlet>>();
		qRegisterMetaType<std::shared_ptr<hm::NodeRenderer>>();
		qRegisterMetaType<std::shared_ptr<hm::BaseParameter>>();
		qRegisterMetaType<hm::NodePtr>("NodePtr");
		qRegisterMetaType<hm::PatchCordPtr>("PatchCordPtr");
		qRegisterMetaType<hm::OutletPtr>("OutletPtr");
		qRegisterMetaType<hm::InletPtr>("InletPtr");
		qRegisterMetaType<hm::NodeRendererPtr>("NodeRendererPtr");
		qRegisterMetaType<hm::ParameterPtr>("ParameterPtr");
		isRegistered = true;
	}
}