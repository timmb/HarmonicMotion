#include "Event.h"
#include "Listener.h"
#include "PatchCord.h"

using namespace hm;

void NodeAddedEvent::notify(Listener* listener)
{
	listener->nodeAdded(node);
}

void NodeRemovedEvent::notify(Listener* listener)
{
	listener->nodeRemoved(node);
}

void NodeCharacteristicsChangedEvent::notify(Listener* listener)
{
	listener->nodeCharacteristicsChanged(node);
}

void NodeParamsChangedEvent::notify(Listener* listener)
{
	listener->nodeParamsChanged(node);
}

void PatchCordAddedEvent::notify(Listener* listener)
{
	listener->patchCordAdded(patchCord->outlet(), patchCord->inlet());
}

void PatchCordRemovedEvent::notify(Listener* listener)
{
	listener->patchCordRemoved(patchCord->outlet(), patchCord->inlet());
}

void ParameterChangedExternallyEvent::notify(Listener* listener)
{
	listener->parameterChangedExternally(parameter);
}

void ParameterChangedInternallyEvent::notify(Listener* listener)
{
	listener->parameterChangedInternally(parameter);
}

void LoadFromJsonCompleteEvent::notify(Listener* listener)
{
	listener->loadFromJsonComplete(errors);
}

void ClearCompleteEvent::notify(Listener* listener)
{
	listener->clearComplete();
}