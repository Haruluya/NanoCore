#pragma once
#include "NanoCore.h"
#include "EditorLayer.h"
#include <filesystem>



namespace NanoCore {

	class ViewportPanel
	{
	public:
		ViewportPanel(Shared<EditorLayer> editor) { m_layerState = editor; }

		void OnImGuiRender();


	private:
		Shared<EditorLayer> m_layerState;
	};

}