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

void PatchCordAddedEvent::notify(Listener* listener)
{
	listener->patchCordAdded(patchCord->outlet(), patchCord->inlet());
}

void PatchCordRemovedEvent::notify(Listener* listener)
{
	listener->patchCordRemoved(patchCord->outlet(), patchCord->inlet());
}

