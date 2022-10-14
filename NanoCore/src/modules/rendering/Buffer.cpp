#include "ncpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "platform/opengl/OpenGLBuffer.h"

namespace NanoCore{

	Shared<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    NANO_ENGINE_LOG_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return Shared<OpenGLVertexBuffer>::Create(size);
		}

		NANO_ENGINE_LOG_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	Shared<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		//case RendererAPI::None:    NANO_ENGINE_LOG_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		//case RendererAPI::OpenGL:  return new OpenGLVertexBuffer(vertices, size);
		case RendererAPI::API::None:    NANO_ENGINE_LOG_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return Shared<OpenGLVertexBuffer>::Create(vertices, size);
		}

		NANO_ENGINE_LOG_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Shared<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		//case RendererAPI::None:    NANO_ENGINE_LOG_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		//case RendererAPI::OpenGL:  return new OpenGLIndexBuffer(indices, size);
		case RendererAPI::API::None:    NANO_ENGINE_LOG_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return Shared<OpenGLIndexBuffer>::Create(indices, size);
		}

		NANO_ENGINE_LOG_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}