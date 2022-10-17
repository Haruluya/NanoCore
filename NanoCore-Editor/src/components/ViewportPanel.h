#pragma once
#include "NanoCore.h"
#include "EditorLayer.h"
#include <filesystem>



namespace NanoCore {

	class ViewportPanel : public NanoPanel
	{
	public:
		ViewportPanel() {};

		virtual void OnUIRender(bool& isOpen) override;


	private:
	};

}