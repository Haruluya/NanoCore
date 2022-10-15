#pragma once

#include "NanoCore.h"
#include "stb_image/stb_image.h"


namespace NanoCore {

	class ResourcesLoadFactory
	{
	public:
		//inline static Shared<Texture2D> LogoTexture = nullptr;

		static void Init()
		{
			// Generic
			//LogoTexture = LoadTexture("haruluya.jpg", { "logoIcon", TextureWrap::Clamp });
			
	
		}

		static void Shutdown()
		{
		
		}
		static bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height, int set_width, int set_height) {
			// Load from file
			int image_width = 0;
			int image_height = 0;

			unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
			if (set_width == 0 || set_height == 0) {
				set_width = image_width;
				set_height = image_height;
			}
			if (image_data == NULL)
				return false;

			// Create a OpenGL texture identifier
			GLuint image_texture;
			glGenTextures(1, &image_texture);
			glBindTexture(GL_TEXTURE_2D, image_texture);

			// Setup filtering parameters for display
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

			// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, set_width, set_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
			stbi_image_free(image_data);

			*out_texture = image_texture;
			*out_width = image_width;
			*out_height = image_height;

			return true;
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
