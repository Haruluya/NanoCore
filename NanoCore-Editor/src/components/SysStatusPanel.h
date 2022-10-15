#pragma once
#include "NanoCore.h"

#include <filesystem>

namespace NanoCore {

	class SysStatusPanel
	{
	public:
		SysStatusPanel();

		void OnImGuiRender();


	private:
		Entity m_HoveredEntity;
	};

}