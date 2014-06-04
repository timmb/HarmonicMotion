#pragma once
#include <QObject>
#include "Pipeline.h"

namespace hm
{
	class PipelineListener : public QObject, public Pipeline::Listener
	{
		Q_OBJECT
		
	public:
		PipelineListener(QObject* parent = nullptr);
		
		virtual void nodeAdded(NodePtr node) override;
		/// Nodes are guaranteed not to be removed if they are still
		/// referenced by active patchcords (equivalently if they still
		/// hold reference to any patchcords).
		virtual void nodeRemoved(NodePtr node) override;
		virtual void patchCordAdded(OutletPtr outlet, InletPtr inlet) override;
		virtual void patchCordRemoved(OutletPtr outlet, InletPtr inlet) override;
		
	Q_SIGNALS:
		void sigNodeAdded(NodePtr node);
		void sigNodeRemoved(NodePtr node);
		void sigPatchCordAdded(OutletPtr outlet, InletPtr inlet);
		void sigPatchCordRemoved(OutletPtr outlet, InletPtr inlet);
	};
}