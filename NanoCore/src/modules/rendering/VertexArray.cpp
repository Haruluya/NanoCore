#include "ncpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "platform/opengl/OpenGLVertexArray.h"

namespace NanoCore{

	Shared<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    NANO_ENGINE_LOG_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return Shared<OpenGLVertexArray>::Create();
		}

		NANO_ENGINE_LOG_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}