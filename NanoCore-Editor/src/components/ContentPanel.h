#pragma once
#include "NanoCore.h"
#include "modules/rendering/Texture.h"
#include "NanoPanel.h"
#include <filesystem>

namespace NanoCore {

	class ContentPanel : public NanoPanel
	{
	public:
		ContentPanel();

		virtual void OnUIRender(bool& isOpen) override;


		//static float m_padding;
		//static float m_thumbnailSize;
	private:
		std::filesystem::path m_CurrentDirectory;

		Shared<Texture2D> m_DirectoryIcon;
		Shared<Texture2D> m_FileIcon;

	};

}