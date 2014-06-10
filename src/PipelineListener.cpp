#include "PipelineListener.h"
using namespace hm;

PipelineListener::PipelineListener(QObject* parent)
: QObject(parent)
{}

void PipelineListener::nodeAdded(NodePtr node)
{
	Q_EMIT sigNodeAdded(node);
}

void PipelineListener::nodeRemoved(NodePtr node)
{
	Q_EMIT sigNodeRemoved(node);
}

void PipelineListener::nodeParamsChanged(NodePtr node)
{
	Q_EMIT sigNodeParamsChanged(node);
}

void PipelineListener::nodeCharacteristicsChanged(NodePtr node)
{
	Q_EMIT sigNodeCharacteristicsChanged(node);
}

void PipelineListener::patchCordAdded(OutletPtr outlet, InletPtr inlet)
{
	Q_EMIT sigPatchCordAdded(outlet, inlet);
}

void PipelineListener::patchCordRemoved(OutletPtr outlet, InletPtr inlet)
{
	Q_EMIT sigPatchCordRemoved(outlet, inlet);
}