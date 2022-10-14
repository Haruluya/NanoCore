#pragma once
#include "RenderCommand.h"
#include "modules/entity/OrthographicCamera.h"
#include "Shader.h"
namespace NanoCore{



	class Renderer
	{
	public:
		static void Init();
		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void Submit(Shared<Shader>& shader, Shared<VertexArray>& vertexArray, const glm::mat4& transform);
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* s_SceneData;
	};


}