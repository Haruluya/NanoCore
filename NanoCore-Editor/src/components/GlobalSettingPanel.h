#pragma once
#include "NanoCore.h"

#include <filesystem>

namespace NanoCore {

	class GlobalSettingPanel
	{
	public:
		GlobalSettingPanel();

		void OnImGuiRender();


	private:
		Entity m_HoveredEntity;
	};

}