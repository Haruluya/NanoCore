#pragma once




#include "NanoCore.h"

namespace NanoCore {

	class UITestLayer : public Layer
	{
	public:
		UITestLayer();
		virtual ~UITestLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
	private:
		inline static bool m_TitleBarHovered = false;


		//OrthographicCameraController m_CameraController;


		// Temp
		Shared<VertexArray> m_SquareVA;
		Shared<Shader> m_FlatColorShader;
		Shared<Framebuffer> m_Framebuffer;

		Shared<Scene> m_ActiveScene;
		Shared<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;

		Entity m_HoveredEntity;

		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];
	};

}