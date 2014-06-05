/// Internal classes used to represent pipeline events
#pragma once
#include "Common.h"

namespace hm
{
	class Listener;
	
	struct Event
	{
		/// Notify \p listener of the event associated with this object
		virtual void notify(Listener* listener) = 0;
	};
	typedef std::shared_ptr<Event> EventPtr;
	typedef std::vector<EventPtr> Events;
	
	struct NodeAddedEvent : public Event
	{
		NodePtr node;
		
		NodeAddedEvent(NodePtr node_) : node(node_) {}
		virtual void notify(Listener* listener);
	};
	
	struct NodeRemovedEvent : public Event
	{
		NodePtr node;
		
		NodeRemovedEvent(NodePtr node_) : node(node_) {}
		virtual void notify(Listener* listener);
	};
	
	struct NodeCharacteristicsChangedEvent : public Event
	{
		NodePtr node;
		
		NodeCharacteristicsChangedEvent(NodePtr node_) : node(node_) {}
		virtual void notify(Listener* listener);
	};
	
	struct PatchCordAddedEvent : public Event
	{
		PatchCordPtr patchCord;
		
		PatchCordAddedEvent(PatchCordPtr patchCord_) : patchCord(patchCord_) {}
		virtual void notify(Listener* listener);
	};
	
	struct PatchCordRemovedEvent : public Event
	{
		PatchCordPtr patchCord;
		
		PatchCordRemovedEvent(PatchCordPtr patchCord_) : patchCord(patchCord_) {}
		virtual void notify(Listener* listener);
	};
}