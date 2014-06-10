#pragma once
#include "Common.h"

namespace hm
{
	/// Override this class and register it with Pipeline to be
	/// notified of updates.
	class Listener
	{
	public:
		virtual void nodeAdded(NodePtr node) {}
		/// Nodes are guaranteed not to be removed if they are still
		/// referenced by active patchcords (equivalently if they still
		/// hold reference to any patchcords).
		virtual void nodeRemoved(NodePtr node) {}
		/// Node characterics refers to the number of inlets or outlets
		/// on a node. Any patch cords that need to be deleted as a
		/// consequence will have aready been notified to this listener.
		virtual void nodeCharacteristicsChanged(NodePtr node) {}
		/// Node Params is the Node::Params struct that defines a node's
		/// name, gui location, etc.
		virtual void nodeParamsChanged(NodePtr node) {}
		virtual void patchCordAdded(OutletPtr outlet, InletPtr inlet) {}
		virtual void patchCordRemoved(OutletPtr outlet, InletPtr inlet) {}
	};

}