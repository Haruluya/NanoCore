#pragma once

#include "NanoCore.h"
#include "stb_image/stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace NanoCore {

	class ResourcesLoadFactory
	{
	public:
		inline static Shared<Texture2D> LogoTexture = nullptr;
		inline static Shared<Texture2D> MinimizeIcon = nullptr;
		inline static Shared<Texture2D> MaximizeIcon = nullptr;
		inline static Shared<Texture2D> RestoreIcon = nullptr;
		inline static Shared<Texture2D> CloseIcon = nullptr;
		static void Init()
		{
			// Generic
			LogoTexture = LoadTexture("ui/nanocore.png", { "logoIcon", TextureWrap::Clamp });
			MinimizeIcon = LoadTexture("ui/Minimize.png", { "logoIcon", TextureWrap::Clamp });
			MaximizeIcon = LoadTexture("ui/Maximize.png", { "logoIcon", TextureWrap::Clamp });
			RestoreIcon = LoadTexture("ui/Restore.png", { "logoIcon", TextureWrap::Clamp });
			CloseIcon = LoadTexture("ui/Close.png", { "logoIcon", TextureWrap::Clamp });
		}

		static void Shutdown()
		{
		
		}

	private:
		static Shared<Texture2D> LoadTexture(const std::filesystem::path& relativePath, TextureProperties properties = TextureProperties())
		{

			std::filesystem::path path = std::filesystem::path("resources") / "images" / relativePath;

			if (!FileSystem::Exists(path))
			{

				return nullptr;
			}

			return Texture2D::Create(path.string(), properties);
		}

	};

}
