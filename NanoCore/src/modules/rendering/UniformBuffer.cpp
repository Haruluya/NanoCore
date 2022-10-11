#include "ncpch.h"
#include "UniformBuffer.h"

#include "modules/rendering/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace NanoCore{

	Shared<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    RA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLUniformBuffer>(size, binding);
		}

		RA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}