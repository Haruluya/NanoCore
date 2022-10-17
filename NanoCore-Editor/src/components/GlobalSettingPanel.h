#pragma once
#include "NanoCore.h"
#include "NanoPanel.h"
#include <filesystem>

namespace NanoCore {

	class GlobalSettingPanel : public NanoPanel
	{
	public:
		GlobalSettingPanel();

		virtual void OnUIRender(bool& isOpen) override;


	private:
		Entity m_HoveredEntity;
	};

}