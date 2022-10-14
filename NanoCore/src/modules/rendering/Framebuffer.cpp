#include "ncpch.h"
#include "Framebuffer.h"

#include "modules/rendering/Renderer.h"

#include "platform/opengl/OpenGLFramebuffer.h"

namespace NanoCore{

	Shared<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    NANO_ENGINE_LOG_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return Shared<OpenGLFramebuffer>::Create(spec);
		}

		NANO_ENGINE_LOG_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}


}
