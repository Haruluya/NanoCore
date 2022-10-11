#include "ncpch.h"
#include "RenderCommand.h"

#include "platform/opengl/OpenGLRendererAPI.h"

namespace NanoCore{

	Unique<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}