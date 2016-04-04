#include "PipelineListener.h"
#include "Parameter.h"
#include <QDebug>
#include "Utilities.h"
using namespace hm;

PipelineListener::PipelineListener(QObject* parent)
: QObject(parent)
{}

void PipelineListener::nodeAdded(NodePtr node)
{
	qDebug()<<"* PipelineListener::nodeAdded";
	Q_EMIT sigNodeAdded(node);
}

void PipelineListener::nodeRemoved(NodePtr node)
{
	qDebug()<<"* PipelineListener::nodeRemoved";
	Q_EMIT sigNodeRemoved(node);
}

void PipelineListener::nodeParamsChanged(NodePtr node)
{
	qDebug()<<"* PipelineListener::nodeParamsChanged";
	Q_EMIT sigNodeParamsChanged(node);
}

void PipelineListener::nodeCharacteristicsChanged(NodePtr node)
{
	qDebug()<<"* PipelineListener::nodeCharacteristicsChanged";
	Q_EMIT sigNodeCharacteristicsChanged(node);
}

void PipelineListener::patchCordAdded(OutletPtr outlet, InletPtr inlet)
{
	qDebug()<<"* PipelineListener::patchCordAdded";
	Q_EMIT sigPatchCordAdded(outlet, inlet);
}

void PipelineListener::patchCordRemoved(OutletPtr outlet, InletPtr inlet)
{
	qDebug()<<"* PipelineListener::patchCordRemoved";
	Q_EMIT sigPatchCordRemoved(outlet, inlet);
}

void PipelineListener::parameterChangedExternally(ParameterPtr parameter)
{
	qDebug()<<"* PipelineListener::parameterChangedExternally";
	Q_EMIT sigParameterChangedExternally(parameter);
}

void PipelineListener::parameterChangedInternally(ParameterPtr parameter)
{
	qDebug()<<"* PipelineListener::parameterChangedInternally";
	Q_EMIT sigParameterChangedInternally(parameter);
}

void PipelineListener::loadFromJsonComplete(std::vector<std::string> const& errors)
{
	hm_debug("* PipelineListener::loadFromJsonComplete");
	QStringList qErrors;
	for (std::string const& s: errors)
	{
		qErrors << str(s);
	}
	Q_EMIT sigLoadFromJsonComplete(qErrors);
}


void PipelineListener::clearComplete()
{
	hm_debug("* PipelineListener::clearComplete");
	Q_EMIT sigClearComplete();
}