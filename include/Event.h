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
		virtual void notify(Listener* listener) override;
	};
	
	struct NodeRemovedEvent : public Event
	{
		NodePtr node;
		
		NodeRemovedEvent(NodePtr node_) : node(node_) {}
		virtual void notify(Listener* listener) override;
	};
	
	struct NodeCharacteristicsChangedEvent : public Event
	{
		NodePtr node;
		
		NodeCharacteristicsChangedEvent(NodePtr node_) : node(node_) {}
		virtual void notify(Listener* listener) override;
	};
	
	struct NodeParamsChangedEvent : public Event
	{
		NodePtr node;
		
		NodeParamsChangedEvent(NodePtr node_) : node(node_) {}
		virtual void notify(Listener* listener) override;
	};
	
	struct PatchCordAddedEvent : public Event
	{
		PatchCordPtr patchCord;
		
		PatchCordAddedEvent(PatchCordPtr patchCord_) : patchCord(patchCord_) {}
		virtual void notify(Listener* listener) override;
	};
	
	struct PatchCordRemovedEvent : public Event
	{
		PatchCordPtr patchCord;
		
		PatchCordRemovedEvent(PatchCordPtr patchCord_) : patchCord(patchCord_) {}
		virtual void notify(Listener* listener) override;
	};
	
	struct ParameterChangedExternallyEvent : public Event {
		ParameterPtr parameter;
		
		ParameterChangedExternallyEvent(ParameterPtr p) : parameter(p) {}
		virtual void notify(Listener* listener) override;
	};
	
	struct ParameterChangedInternallyEvent : public Event {
		ParameterPtr parameter;
		
		ParameterChangedInternallyEvent(ParameterPtr p) : parameter(p) {}
		virtual void notify(Listener* listener) override;
	};
	
	struct LoadFromJsonCompleteEvent : public Event {
		/// If errors.empty() then the load was successful
		std::vector<std::string> errors;
		
		LoadFromJsonCompleteEvent(std::vector<std::string> e) : errors(e) {}
		virtual void notify(Listener* listener) override;
	};

}









