#include "ncpch.h"
#include "Framebuffer.h"

#include "modules/rendering/Renderer.h"

#include "platform/opengl/OpenGLFramebuffer.h"

namespace NanoCore{

	Shared<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    RA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLFramebuffer>(spec);
		}

		RA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}


}
