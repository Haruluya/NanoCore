#include "ncpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "platform/opengl/OpenGLVertexArray.h"

namespace NanoCore{

	Shared<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    RA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLVertexArray>();
		}

		RA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}