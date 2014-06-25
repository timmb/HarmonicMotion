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
		virtual std::string name() const = 0;
	};
	typedef std::shared_ptr<Event> EventPtr;
	typedef std::vector<EventPtr> Events;
	
	struct NodeAddedEvent : public Event
	{
		NodePtr node;
		
		NodeAddedEvent(NodePtr node_) : node(node_) {}
		virtual void notify(Listener* listener) override;
		virtual std::string name() const { return "NodeAddedEvent"; }
	};
	
	struct NodeRemovedEvent : public Event
	{
		NodePtr node;
		
		NodeRemovedEvent(NodePtr node_) : node(node_) {}
		virtual void notify(Listener* listener) override;
		virtual std::string name() const { return "NodeRemovedEvent"; }
	};
	
	struct NodeCharacteristicsChangedEvent : public Event
	{
		NodePtr node;
		
		NodeCharacteristicsChangedEvent(NodePtr node_) : node(node_) {}
		virtual void notify(Listener* listener) override;
		virtual std::string name() const { return "NodeCharacteristicsChangedEvent"; }
	};
	
	struct NodeParamsChangedEvent : public Event
	{
		NodePtr node;
		
		NodeParamsChangedEvent(NodePtr node_) : node(node_) {}
		virtual void notify(Listener* listener) override;
		virtual std::string name() const { return "NodeParamsChangedEvent"; }
	};
	
	struct PatchCordAddedEvent : public Event
	{
		PatchCordPtr patchCord;
		
		PatchCordAddedEvent(PatchCordPtr patchCord_) : patchCord(patchCord_) {}
		virtual void notify(Listener* listener) override;
		virtual std::string name() const { return "PatchCordAddedEvent"; }
	};
	
	struct PatchCordRemovedEvent : public Event
	{
		PatchCordPtr patchCord;
		
		PatchCordRemovedEvent(PatchCordPtr patchCord_) : patchCord(patchCord_) {}
		virtual void notify(Listener* listener) override;
		virtual std::string name() const { return "PatchCordRemovedEvent"; }
	};
	
	struct ParameterChangedExternallyEvent : public Event {
		ParameterPtr parameter;
		
		ParameterChangedExternallyEvent(ParameterPtr p) : parameter(p) {}
		virtual void notify(Listener* listener) override;
		virtual std::string name() const { return "ParameterChangedExternallyEvent"; }
	};
	
	struct ParameterChangedInternallyEvent : public Event {
		ParameterPtr parameter;
		
		ParameterChangedInternallyEvent(ParameterPtr p) : parameter(p) {}
		virtual void notify(Listener* listener) override;
		virtual std::string name() const { return "ParameterChagnedInternallyEvent"; }
	};
	
	struct LoadFromJsonCompleteEvent : public Event {
		/// If errors.empty() then the load was successful
		std::vector<std::string> errors;
		
		LoadFromJsonCompleteEvent(std::vector<std::string> e) : errors(e) {}
		virtual void notify(Listener* listener) override;
		virtual std::string name() const { return "LoadFromJsonCompleteEvent"; }
	};

}









