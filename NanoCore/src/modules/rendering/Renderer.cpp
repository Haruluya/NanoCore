#include "ncpch.h"
#include "Renderer.h"
#include "platform/opengl/OpenGLShader.h"
#include "modules/utils/RenderUtils.h"
namespace NanoCore{



	
	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RA_PROFILE_FUNCTION();
		RenderCommand::Init();
		RenderUtils::Init();
	}
	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}
	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(Shared<Shader>& shader, Shared<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->SetMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::Shutdown()
	{
		RenderUtils::Shutdown();
	}
}