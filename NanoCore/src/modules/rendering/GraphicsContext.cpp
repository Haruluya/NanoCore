#include "ncpch.h"
#include "GraphicsContext.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace NanoCore{

	Unique<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    RA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_unique<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		RA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}