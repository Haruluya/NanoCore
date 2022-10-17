#pragma once
#include "NanoCore.h"
#include "NanoPanel.h"
#include "EditorLayer.h"
#include <filesystem>

namespace NanoCore {

	class SysStatusPanel : public NanoPanel
	{
	public:
		SysStatusPanel();

		virtual void OnUIRender(bool& isOpen) override;



	private:

	};

}