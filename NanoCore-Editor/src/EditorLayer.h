#pragma once

#include "NanoCore.h"
#include "components/HierarchyPanel.h"
#include "components/ContentPanel.h"
#include "components/SysStatusPanel.h"
#include "components/ViewportPanel.h"
#include "components/GlobalSettingPanel.h"

#include "modules/entity/EditorCamera.h"

namespace NanoCore {
	enum ThemeColor
	{
		THEME_BLUE, THEME_DARK, THEME_GRAY, THEME_LIGHT
	};
	class EditorLayer : public Layer, RefCount
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void OnOverlayRender();

		void NewScene();

		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(Shared<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();


		void StyleSetting();

		void OnDuplicateEntity();

		void UITitlebar();
		void UIMainMenu();

		// UI Panels
		void UI_Toolbar();
	private:
		ThemeColor m_ThemeColor = THEME_DARK;

		NanoCore::OrthographicCameraController m_CameraController;

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



		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;

		Shared<Texture2D> m_CheckerboardTexture;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];
		bool m_ViewportFocused = false, m_ViewportHovered = false;

		bool m_StartedRightClickInViewport = false;





		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		int m_GizmoType = -1;

		bool m_ShowPhysicsColliders = false;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;

		// Panels
		HierarchyPanel m_HierarchyPanel;
		ContentPanel m_ContentPanel;
		ViewportPanel m_ViewportPanel;
		SysStatusPanel m_SysStatusPanel;
		GlobalSettingPanel m_GlobalSettingPanel;

		// Editor resources
		Shared<Texture2D> m_IconPlay, m_IconSimulate, m_IconStop;


		bool m_show_style_setting = false;
		bool m_TitleBarHovered = false;


	private:
		friend class ViewportPanel;
		friend class HierarchyPanel;
		friend class ContentPanel;
	};

}