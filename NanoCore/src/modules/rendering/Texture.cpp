#include "ncpch.h"
#include "modules/rendering/Texture.h"

#include "modules/rendering/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace NanoCore{
	Shared<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    NANO_ENGINE_LOG_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return Shared<OpenGLTexture2D>::Create(width, height);
		}

		NANO_ENGINE_LOG_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	Shared<Texture2D> Texture2D::Create(ImageFormat format, uint32_t width, uint32_t height, const void* data, TextureProperties properties)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    NANO_ENGINE_LOG_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return Shared<OpenGLTexture2D>::Create(width, height);
		}

		NANO_ENGINE_LOG_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Shared<Texture2D> Texture2D::Create(const std::string& path, TextureProperties properties)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    NANO_ENGINE_LOG_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return Shared<OpenGLTexture2D>::Create(path);
		}

		NANO_ENGINE_LOG_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	//--------------------------!


}