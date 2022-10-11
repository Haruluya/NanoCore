#pragma once



#include "core/layer/Layer.h"

#include "modules/events/ApplicationEvent.h"
#include "modules/events/EventTypes.h"

namespace NanoCore{

	class UILayer : public Layer
	{
	public:
		UILayer();
		~UILayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event & e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }


	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;

		bool show_demo = true;
	};
}