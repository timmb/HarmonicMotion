#pragma once
#include "Common.h"

namespace hm
{
	/// Override this class and register it with Pipeline to be
	/// notified of updates.
	/// \note The functions of this class may be called from different
	/// threads.
	class Listener
	{
	public:
		virtual void nodeAdded(NodePtr node) {}
		
		/// Nodes are guaranteed not to be removed if they are still
		/// referenced by active patchcords (equivalently if they still
		/// hold reference to any patchcords).
		virtual void nodeRemoved(NodePtr node) {}
		
		/// Node characterics refers to the number of inlets or outlets
		/// on a node. All patch cords will have been deleted from the
		/// node by the time this is called, and if applicable will be
		/// recreated afterwards.
		virtual void nodeCharacteristicsChanged(NodePtr node) {}
		
		/// Node Params is the Node::Params struct that defines a node's
		/// name, gui location, etc.
		virtual void nodeParamsChanged(NodePtr node) {}
		
		virtual void patchCordAdded(OutletPtr outlet, InletPtr inlet) {}
		
		virtual void patchCordRemoved(OutletPtr outlet, InletPtr inlet) {}
		
		/// A parameter change is described as external if the change
		/// happens from a client using Parameter<T>::set
		virtual void parameterChangedExternally(ParameterPtr parameter) {}
		
		/// A parameter change is described as internal if the change
		/// happens when a node changes a value that is observed by
		/// a parameter
		virtual void parameterChangedInternally(ParameterPtr parameter) {}
		
		/// This is triggered when a load from Json completes.
		/// \param errors If empty then the load was successful, otherwise
		/// a list of error messages.
		virtual void loadFromJsonComplete(std::vector<std::string> const& errors) {}
	};

}