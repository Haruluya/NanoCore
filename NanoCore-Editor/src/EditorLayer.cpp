#include "EditorLayer.h"
#include "modules/entity/SceneSerializer.h"
#include "modules/utils/PlatformUtils.h"
#include "core/math/NanoMath.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGuizmo/ImGuizmo.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "utils/ResourcesLoadFactory.h"

#include "ui/NanoCoreUI.hpp"


namespace NanoCore {

	void SetThemeColors(ThemeColor);
	void SetBlueThemeColors();
	void SetGrayThemeColors();
	void SetDarkThemeColors();
	void SetLightThemeColors();


	EditorLayer* EditorLayer::m_EditorInstance = nullptr;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
	{
		m_EditorInstance = this;
	}

	void EditorLayer::OnAttach()
	{
		RA_PROFILE_FUNCTION();

		// Init resources.
		ResourcesLoadFactory::Init();

		// Create panel manager.
		m_PanelManager = std::make_unique<PanelManager>();

		m_PanelManager->AddPanel<HierarchyPanel>(PanelCategory::Edit, "HierarchyPanel", "Hierarchy", true);
		m_HierarchyPanel = m_PanelManager->GetPanel<HierarchyPanel>("HierarchyPanel");

		m_PanelManager->AddPanel<ViewportPanel>(PanelCategory::Edit, "ViewPortPanel", "ViewPort", true);
		m_PanelManager->AddPanel<SysStatusPanel>(PanelCategory::Edit, "SysStatusPanel", "SysStatus", true);
		m_PanelManager->AddPanel<ContentPanel>(PanelCategory::Edit, "ContentPanel", "Content", true);
		m_PanelManager->AddPanel<GlobalSettingPanel>(PanelCategory::Edit, "GlobalSettingPanel", "GlobalSetting", true);

		m_CheckerboardTexture = Texture2D::Create("resources/textures/empty.png");
		m_IconPlay = Texture2D::Create("resources/icons/Play.png");
		m_IconSimulate = Texture2D::Create("resources/icons/Simulate.png");
		m_IconStop = Texture2D::Create("resources/icons/Stop.png");

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_EditorScene = Shared<Scene>::Create();
		m_ActiveScene = m_EditorScene;

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(sceneFilePath);
		}

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		RenderUtils::SetLineWidth(4.0f);



	}

	void EditorLayer::OnDetach()
	{
		RA_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		RA_PROFILE_FUNCTION();

		// Resize
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Render
		RenderUtils::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		// Clear our entity ID attachment to -1
		m_Framebuffer->ClearAttachment(1, -1);

		switch (m_SceneState)
		{
		case SceneState::Edit:
		{
			if (m_ViewportFocused)
				m_CameraController.OnUpdate(ts);

			m_EditorCamera.OnUpdate(ts);

			m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
			break;
		}
		case SceneState::Simulate:
		{
			m_EditorCamera.OnUpdate(ts);

			m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera);
			break;
		}
		case SceneState::Play:
		{
			m_ActiveScene->OnUpdateRuntime(ts);
			break;
		}
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.Raw());
		}

		OnOverlayRender();

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		RA_PROFILE_FUNCTION();

		/*
			Set main layout.
		*/
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		auto boldFont = io.Fonts->Fonts[0];
		auto largeFont = io.Fonts->Fonts[1];

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !m_StartedRightClickInViewport))
		{
			if (m_SceneState != SceneState::Play)
			{
				ImGui::FocusWindow(GImGui->HoveredWindow);
			}
		}

		io.ConfigWindowsResizeFromEdges = io.BackendFlags & ImGuiBackendFlags_HasMouseCursors;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		bool isMaximized = (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, isMaximized ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleColor(); // MenuBarBg
		ImGui::PopStyleVar(2);

		ImGui::PopStyleVar(2);




		/*
			Set style configuration.
		*/
		StyleSetting();


		/*
			Set title bar.
		*/
		const float titlebarHeight = 55.0f;
		UITitlebar(titlebarHeight);
		ImGui::SetCursorPosY(titlebarHeight + ImGui::GetCurrentWindow()->WindowPadding.y);


		/*
			DockSpace.
		*/
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		ImGui::DockSpace(ImGui::GetID("Dockspace"));
		style.WindowMinSize.x = minWinSizeX;

		/*
			Set panels.
		*/
		m_PanelManager->OnUIRender();

		/*
			Set tool bar.
		*/
		UI_Toolbar();


		ImGui::End();
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = (bool)m_ActiveScene;

		ImVec4 tintColor = ImVec4(1, 1, 1, 1);
		if (!toolbarEnabled)
			tintColor.w = 0.5f;

		float size = ImGui::GetWindowHeight() - 4.0f;
		{
			Shared<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}
		}
		ImGui::SameLine();
		{
			Shared<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;		//ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
					OnSceneSimulate();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneStop();
			}
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::UITitlebar(float titlebarHeight) {

		const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;


		ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y));
		const ImVec2 titlebarMin = ImGui::GetCursorScreenPos();
		const ImVec2 titlebarMax = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - windowPadding.y * 2.0f,
									 ImGui::GetCursorScreenPos().y + titlebarHeight };
		auto* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(titlebarMin, titlebarMax, UI::Color::titlebar);

		// Logo
		{
			const int logoWidth = ResourcesLoadFactory::LogoTexture->GetWidth();
			const int logoHeight = ResourcesLoadFactory::LogoTexture->GetHeight();
			const ImVec2 logoOffset(16.0f + windowPadding.x, 8.0f + windowPadding.y);
			ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y };
			ImVec2 logoRectMax = { logoRectStart.x + logoWidth, logoRectStart.y + logoHeight };
			logoRectStart.x -= 5.0f;
			logoRectStart.y -= 5.0f;
			logoRectMax.x -= 5.0f;
			logoRectMax.y -= 5.0f;

			drawList->AddImage((ImTextureID)ResourcesLoadFactory::LogoTexture->GetRendererID(), logoRectStart, logoRectMax);
		}

		ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth() - windowPadding.y * 2.0f, ImGui::GetFrameHeightWithSpacing() });

		static float moveOffsetX;
		static float moveOffsetY;
		const float w = ImGui::GetContentRegionAvail().x;
		const float buttonsAreaWidth = 94;

		// Title bar drag area

		// On Windows we hook into the GLFW win32 window internals

		auto* rootWindow = ImGui::GetCurrentWindow()->RootWindow;
		const float windowWidth = (int)rootWindow->RootWindow->Size.x;

		if (ImGui::InvisibleButton("##titleBarDragZone", ImVec2(w - buttonsAreaWidth, titlebarHeight), ImGuiButtonFlags_PressedOnClick))
		{
			ImVec2 point = ImGui::GetMousePos();
			ImRect rect = rootWindow->Rect();
			// Calculate the difference between the cursor pos and window pos
			moveOffsetX = point.x - rect.Min.x;
			moveOffsetY = point.y - rect.Min.y;

		}

		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
		{
			auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
			bool maximized = (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
			if (maximized)
				glfwRestoreWindow(window);
			else
				Application::Get().GetWindow().Maximize();
		}
		else if (ImGui::IsItemActive())
		{
			// TODO: move this stuff to a better place, like Window class
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
				int maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
				if (maximized)
				{
					glfwRestoreWindow(window);

					int newWidth, newHeight;
					glfwGetWindowSize(window, &newWidth, &newHeight);

					// Offset position proportionally to mouse position on titlebar
					// This ensures we dragging window relatively to cursor position on titlebar
					// correctly when window size changes
					if (windowWidth - (float)newWidth > 0.0f)
						moveOffsetX *= (float)newWidth / windowWidth;
				}

				ImVec2 point = ImGui::GetMousePos();
				glfwSetWindowPos(window, point.x - moveOffsetX, point.y - moveOffsetY);

			}
		}


		// Draw Menubar

		UIMainMenu();

		const float menuBarRight = ImGui::GetItemRectMax().x - ImGui::GetCurrentWindow()->Pos.x;

		// Project name
		{

			UI::ScopedColour textColour(ImGuiCol_Text, UI::Color::textDarker);
			UI::ScopedColour border(ImGuiCol_Border, IM_COL32(40, 40, 40, 255));

			const std::string title = Project::GetActive()->GetConfig().Name + " Version:1.0.0";
			const ImVec2 textSize = ImGui::CalcTextSize(title.c_str());
			const float rightOffset = ImGui::GetWindowWidth() / 5.0f;
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - rightOffset - textSize.x);
			UI::ShiftCursorY(1.0f + windowPadding.y);

			{
				UI::ScopedFont boldFont(ImGui::GetIO().Fonts->Fonts[0]);
				ImGui::Text(title.c_str());
			}
			UI::SetTooltip("NanoCore By Haruluya.");


			UI::DrawBorder(UI::RectExpanded(UI::GetItemRect(), 24.0f, 68.0f), 1.0f, 3.0f, 0.0f, -60.0f);
		}

		// Current Scene name
		{
			UI::ScopedColour textColour(ImGuiCol_Text, UI::Color::text);
			const std::string sceneName = "Haruluya";

			ImGui::SetCursorPosX(menuBarRight);
			UI::ShiftCursorX(50.0f);

			{
				UI::ScopedFont boldFont(ImGui::GetIO().Fonts->Fonts[0]);
				ImGui::Text(sceneName.c_str());
			}
			UI::SetTooltip("Current scene ()");

			const float underlineThickness = 2.0f;
			const float underlineExpandWidth = 4.0f;

			ImRect itemRect = UI::RectExpanded(UI::GetItemRect(), underlineExpandWidth, 0.0f);


			// Horizontal line
			itemRect.Min.y = itemRect.Max.y - underlineThickness;
			itemRect = UI::RectOffset(itemRect, 0.0f, underlineThickness * 2.0f);

			// Vertical line
			//itemRect.Max.x = itemRect.Min.x + underlineThickness;
			//itemRect = UI::RectOffset(itemRect, -underlineThickness * 2.0f, 0.0f);
			drawList->AddRectFilled(itemRect.Min, itemRect.Max, UI::Color::muted, 2.0f);
		}
		ImGui::ResumeLayout();

		// Window buttons
		const ImU32 buttonColN = UI::ColourWithMultipliedValue(UI::Color::text, 0.9f);
		const ImU32 buttonColH = UI::ColourWithMultipliedValue(UI::Color::text, 1.2f);
		const ImU32 buttonColP = UI::Color::textDarker;
		const float buttonWidth = 14.0f;
		const float buttonHeight = 14.0f;

		// Minimize Button

		ImGui::Spring();
		UI::ShiftCursorY(8.0f);
		{
			const int iconWidth = ResourcesLoadFactory::MinimizeIcon->GetWidth();
			const int iconHeight = ResourcesLoadFactory::MinimizeIcon->GetHeight();
			const float padY = (buttonHeight - (float)iconHeight) / 2.0f;

			if (ImGui::InvisibleButton("Minimize", ImVec2(buttonWidth, buttonHeight)))
			{
				if (auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()))
				{
					Application::Get().QueueEvent([window]() { glfwIconifyWindow(window); });
				}
			}

			UI::DrawButtonImage(ResourcesLoadFactory::MinimizeIcon, buttonColN, buttonColH, buttonColP, UI::RectExpanded(UI::GetItemRect(), 0.0f, -padY));
		}


		// Maximize Button

		ImGui::Spring(-1.0f, 17.0f);
		UI::ShiftCursorY(8.0f);
		{
			const int iconWidth = ResourcesLoadFactory::MaximizeIcon->GetWidth();
			const int iconHeight = ResourcesLoadFactory::MaximizeIcon->GetHeight();

			auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
			bool isMaximized = (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);

			if (ImGui::InvisibleButton("Maximize", ImVec2(buttonWidth, buttonHeight)))
			{
				if (isMaximized)
				{
					Application::Get().QueueEvent([window]() { glfwRestoreWindow(window); });
				}
				else
				{
					Application::Get().QueueEvent([]() { Application::Get().GetWindow().Maximize(); });
				}
			}

			UI::DrawButtonImage(isMaximized ? ResourcesLoadFactory::RestoreIcon : ResourcesLoadFactory::MaximizeIcon, buttonColN, buttonColH, buttonColP);
		}


		// Close Button

		ImGui::Spring(-1.0f, 15.0f);
		UI::ShiftCursorY(8.0f);
		{
			const int iconWidth = ResourcesLoadFactory::CloseIcon->GetWidth();
			const int iconHeight = ResourcesLoadFactory::CloseIcon->GetHeight();
			if (ImGui::InvisibleButton("Close", ImVec2(buttonWidth, buttonHeight)))
			{
				Application::Get().DispatchEvent<WindowCloseEvent>();
			}

			UI::DrawButtonImage(ResourcesLoadFactory::CloseIcon, UI::Color::text, UI::ColourWithMultipliedValue(UI::Color::text, 1.4f), buttonColP);
		}

		ImGui::Spring(-1.0f, 18.0f);
		ImGui::EndHorizontal();

	}



	void EditorLayer::UIMainMenu() {
		ImGui::SuspendLayout();
		{
			auto windowPadding = ImGui::GetCurrentWindow()->WindowPadding;
			ImGui::SetItemAllowOverlap();
			const float logoOffset = 16.0f * 2.0f + 41.0f + windowPadding.x;
			ImGui::SetCursorPos(ImVec2(logoOffset, 4.0f));
			const ImRect menuBarRect = { ImGui::GetCursorPos(), {ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeightWithSpacing()} };

			ImGui::BeginGroup();

			if (UI::BeginMenuBar(menuBarRect))
			{
				bool menuOpen = ImGui::IsPopupOpen("##menubar", ImGuiPopupFlags_AnyPopupId);

				if (menuOpen)
				{
					const ImU32 colActive = UI::ColourWithSaturation(UI::Color::accent, 0.5f);
					ImGui::PushStyleColor(ImGuiCol_Header, colActive);
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colActive);
				}

				auto popItemHighlight = [&menuOpen]
				{
					if (menuOpen)
					{
						ImGui::PopStyleColor(3);
						menuOpen = false;
					}
				};

				auto pushDarkTextIfActive = [](const char* menuName)
				{
					if (ImGui::IsPopupOpen(menuName))
					{
						ImGui::PushStyleColor(ImGuiCol_Text, UI::Color::backgroundDark);
						return true;
					}
					return false;
				};

				const ImU32 colHovered = IM_COL32(0, 0, 0, 80);

				{
					bool colourPushed = pushDarkTextIfActive("File");

					if (ImGui::BeginMenuEx("File","A"))
					{
						ImGui::EndMenu();
					}

					if (colourPushed)
						ImGui::PopStyleColor();
				}

				{
					bool colourPushed = pushDarkTextIfActive("Edit");

					if (ImGui::BeginMenu("Edit"))
					{

						ImGui::EndMenu();
					}

					if (colourPushed)
						ImGui::PopStyleColor();
				}

				{
					bool colourPushed = pushDarkTextIfActive("View");

					if (ImGui::BeginMenu("View"))
					{

						ImGui::EndMenu();
					}

					if (colourPushed)
						ImGui::PopStyleColor();
				}


				{
					bool colourPushed = pushDarkTextIfActive("Tools");

					if (ImGui::BeginMenu("Tools"))
					{
						ImGui::EndMenu();
					}

					if (colourPushed)
						ImGui::PopStyleColor();
				}

				{
					bool colourPushed = pushDarkTextIfActive("Help");

					if (ImGui::BeginMenu("Help"))
					{
						popItemHighlight();
						colourPushed = false;

						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colHovered);

						if (ImGui::MenuItem("About"))

							ImGui::PopStyleColor();
						ImGui::EndMenu();
					}

					if (colourPushed)
						ImGui::PopStyleColor();
				}

				if (menuOpen)
					ImGui::PopStyleColor(2);
			}
			UI::EndMenuBar();

			ImGui::EndGroup();
		}
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
		if (m_SceneState == SceneState::Edit)
		{
			m_EditorCamera.OnEvent(e);
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(NANO_EVENT_BIND(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(NANO_EVENT_BIND(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.IsRepeat())
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
		case Key::N:
		{
			if (control)
				NewScene();

			break;
		}
		case Key::O:
		{
			if (control)
				OpenScene();

			break;
		}
		case Key::S:
		{
			if (control)
			{
				if (shift)
					SaveSceneAs();
				else
					SaveScene();
			}

			break;
		}

		// Scene Commands
		case Key::D:
		{
			if (control)
				OnDuplicateEntity();

			break;
		}

		// Gizmos
		case Key::Q:
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = -1;
			break;
		}
		case Key::W:
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		}
		case Key::E:
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		}
		case Key::R:
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}
		}
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_HierarchyPanel->SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
			if (!camera)
				return;

			RenderUtils::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
		}
		else
		{
			RenderUtils::BeginScene(m_EditorCamera);
		}

		if (m_ShowPhysicsColliders)
		{
			// Box Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::scale(glm::mat4(1.0f), scale);

					RenderUtils::DrawRect(transform, glm::vec4(0, 1, 0, 1));
				}
			}

			// Circle Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0f), scale);

					RenderUtils::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
				}
			}
		}

		// Draw selected entity outline 
		if (Entity selectedEntity = m_HierarchyPanel->GetSelectedEntity())
		{
			const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
			RenderUtils::DrawRect(transform.GetTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
		}

		RenderUtils::EndScene();
	}





	void EditorLayer::NewScene()
	{
		m_ActiveScene = Shared<Scene>::Create();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_HierarchyPanel->SetScene((m_ActiveScene));

		m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("NanoCore Scene (*.nanocore)\0*.nanocore\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();

		if (path.extension().string() != ".nanocore")
		{
			return;
		}

		Shared<Scene> newScene = Shared<Scene>::Create();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_HierarchyPanel->SetScene(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorScenePath.empty())
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("NanoCore Scene (*.nanocore)\0*.nanocore\0");
		if (!filepath.empty())
		{
			SerializeScene(m_ActiveScene, filepath);
			m_EditorScenePath = filepath;
		}
	}

	void EditorLayer::SerializeScene(Shared<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_HierarchyPanel->SetScene(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();

		m_HierarchyPanel->SetScene(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		NANO_ENGINE_LOG_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate);

		if (m_SceneState == SceneState::Play)
			m_ActiveScene->OnRuntimeStop();
		else if (m_SceneState == SceneState::Simulate)
			m_ActiveScene->OnSimulationStop();

		m_SceneState = SceneState::Edit;

		m_ActiveScene = m_EditorScene;

		m_HierarchyPanel->SetScene(m_ActiveScene);
	}

	void EditorLayer::StyleSetting()
	{


		SetThemeColors(m_ThemeColor);
		if (m_show_style_setting) {
			if (ImGui::Begin("Style Setting", &m_show_style_setting)) {
				ImGui::Text("Content config:");
				//ImGui::SliderFloat("Thumbnail Size", &(ContentPanel::m_thumbnailSize), 16, 512);
				//ImGui::SliderFloat("Padding", &(ContentPanel::m_padding), 0, 32);


				ImGui::Text("Theme:");
				static int style_idx = 0;
				if (ImGui::Combo("", &style_idx, "Blue\0Gary\0Dark\0Light\0"))
				{
					switch (style_idx)
					{
					case 0: {m_ThemeColor = THEME_BLUE; } break;
					case 1: {m_ThemeColor = THEME_GRAY; } break;
					case 2: {m_ThemeColor = THEME_DARK; } break;
					case 3: {m_ThemeColor = THEME_LIGHT; } break;
					}
					SetThemeColors(m_ThemeColor);
				}
				ImGui::Text("Customize:");
				if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
				{
					ImGuiStyle& style = ImGui::GetStyle();
					if (ImGui::BeginTabItem("Sizes"))
					{
						ImGui::Text("Main");
						ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
						ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
						ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
						ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
						ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
						ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
						ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
						ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
						ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
						ImGui::Text("Borders");
						ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
						ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
						ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
						ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
						ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
						ImGui::Text("Rounding");
						ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
						ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
						ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
						ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
						ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
						ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
						ImGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
						ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
						ImGui::Text("Alignment");
						ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
						int window_menu_button_position = style.WindowMenuButtonPosition + 1;
						if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
							style.WindowMenuButtonPosition = window_menu_button_position - 1;
						ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
						ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
						ImGui::SameLine();
						ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
						ImGui::SameLine();
						ImGui::Text("Safe Area Padding");
						ImGui::SameLine();
						ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Colors"))
					{
						static int output_dest = 0;
						static bool output_only_modified = true;
						if (ImGui::Button("Export"))
						{
							if (output_dest == 0)
								ImGui::LogToClipboard();
							else
								ImGui::LogToTTY();
							ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;" IM_NEWLINE);
							for (int i = 0; i < ImGuiCol_COUNT; i++)
							{
								const ImVec4& col = style.Colors[i];
								const char* name = ImGui::GetStyleColorName(i);
								if (!output_only_modified || memcmp(&col, &style.Colors[i], sizeof(ImVec4)) != 0)
									ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE,
										name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
							}
							ImGui::LogFinish();
						}
						ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
						ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

						static ImGuiTextFilter filter;
						filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

						static ImGuiColorEditFlags alpha_flags = 0;
						if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None)) { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
						if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview)) { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
						if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();

						ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
						ImGui::PushItemWidth(-160);
						for (int i = 0; i < ImGuiCol_COUNT; i++)
						{
							const char* name = ImGui::GetStyleColorName(i);
							if (!filter.PassFilter(name))
								continue;
							ImGui::PushID(i);
							ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
							if (memcmp(&style.Colors[i], &style.Colors[i], sizeof(ImVec4)) != 0)
							{
								// Tips: in a real user application, you may want to merge and use an icon font into the main font,
								// so instead of "Save"/"Revert" you'd use icons!
								// Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
								ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { style.Colors[i] = style.Colors[i]; }
								ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = style.Colors[i]; }
							}
							ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
							ImGui::TextUnformatted(name);
							ImGui::PopID();
						}
						ImGui::PopItemWidth();
						ImGui::EndChild();

						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Fonts"))
					{
						ImGuiIO& io = ImGui::GetIO();
						ImFontAtlas* atlas = io.Fonts;
						ImGui::ShowFontAtlas(atlas);

						// Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
						// (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
						const float MIN_SCALE = 0.3f;
						const float MAX_SCALE = 2.0f;

						static float window_scale = 1.0f;
						ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
						if (ImGui::DragFloat("window scale", &window_scale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp)) // Scale only this window
							ImGui::SetWindowFontScale(window_scale);
						ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp); // Scale everything
						ImGui::PopItemWidth();

						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Rendering"))
					{
						ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
						ImGui::SameLine();


						ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
						ImGui::SameLine();


						ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
						ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
						ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
						if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

						// When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
						ImGui::DragFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError, 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
						if (ImGui::IsItemActive())
						{
							ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
							ImGui::BeginTooltip();
							ImGui::TextUnformatted("(R = radius, N = number of segments)");
							ImGui::Spacing();
							ImDrawList* draw_list = ImGui::GetWindowDrawList();
							const float min_widget_width = ImGui::CalcTextSize("N: MMM\nR: MMM").x;
							for (int n = 0; n < 8; n++)
							{
								const float RAD_MIN = 5.0f;
								const float RAD_MAX = 70.0f;
								const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

								ImGui::BeginGroup();

								ImGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

								const float canvas_width = min_widget_width > rad * 2.0f ? min_widget_width : rad * 2.0f;
								const float offset_x = floorf(canvas_width * 0.5f);
								const float offset_y = floorf(RAD_MAX);

								const ImVec2 p1 = ImGui::GetCursorScreenPos();
								draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
								ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

								/*
								const ImVec2 p2 = ImGui::GetCursorScreenPos();
								draw_list->AddCircleFilled(ImVec2(p2.x + offset_x, p2.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
								ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));
								*/

								ImGui::EndGroup();
								ImGui::SameLine();
							}
							ImGui::EndTooltip();
						}
						ImGui::SameLine();


						ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
						ImGui::DragFloat("Disabled Alpha", &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f"); ImGui::SameLine();
						ImGui::PopItemWidth();

						ImGui::EndTabItem();
					}

					ImGui::EndTabBar();
				}
				ImGui::End();
			}

		}



	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_HierarchyPanel->GetSelectedEntity();
		if (selectedEntity)
			m_EditorScene->DuplicateEntity(selectedEntity);
	}

	void SetThemeColors(ThemeColor themeColor) {
		switch (themeColor)
		{
		case NanoCore::THEME_BLUE:
			SetBlueThemeColors();
			break;
		case NanoCore::THEME_DARK:
			SetDarkThemeColors();
			break;
		case NanoCore::THEME_GRAY:
			SetGrayThemeColors();
			break;
		case NanoCore::THEME_LIGHT:
			SetLightThemeColors();
			break;
		default:
			SetBlueThemeColors();
			break;
		}
	}

	void SetBlueThemeColors()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.Alpha = 1.0;
		//style.WindowFillAlphaDefault = 0.83;
		//style.ChildWindowRounding = 3;
		style.WindowRounding = 3;
		style.GrabRounding = 1;
		style.GrabMinSize = 20;
		style.FrameRounding = 4;
		const ImVec4 none = {};

		style.Colors[ImGuiCol_BorderShadow] = none;
		style.Colors[ImGuiCol_FrameBgActive] = none;
		style.Colors[ImGuiCol_DockingEmptyBg] = none;
		style.Colors[ImGuiCol_ModalWindowDimBg] = none;
		style.Colors[ImGuiCol_TableRowBg] = none;
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
		style.Colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		style.Colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		style.Colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);

		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);

		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
	}

	void SetGrayThemeColors() {
		ImGui::StyleColorsClassic();
	}

	void SetDarkThemeColors() {
		auto& style = ImGui::GetStyle();


		ImVec4* colors = style.Colors;

		const ImVec4 none = {};

		const float lightness = 1.0f;
		auto rgb = [=](unsigned int r, unsigned int g, unsigned int b, float alpha = 1.0f) {
			return ImVec4(r / 255.0f * lightness, g / 255.0f * lightness, b / 255.0f * lightness, alpha);
		};
		auto grey = [=](unsigned int g, float alpha = 1.0f) {
			return rgb(g, g, g, alpha);
		};

		for (unsigned int i = 0; i != ImGuiCol_COUNT; ++i) {
			colors[i] = rgb(255, 0, 0);
		}


		colors[ImGuiCol_BorderShadow] = none;
		colors[ImGuiCol_FrameBgActive] = none;
		colors[ImGuiCol_Tab] = none;
		colors[ImGuiCol_TabUnfocused] = none;
		colors[ImGuiCol_DockingEmptyBg] = none;
		colors[ImGuiCol_ModalWindowDimBg] = none;
		colors[ImGuiCol_TableRowBg] = none;

		colors[ImGuiCol_Text] = grey(180,0.8f);
		colors[ImGuiCol_TextDisabled] = grey(90, 0.8f);

		colors[ImGuiCol_CheckMark] = rgb(0, 112, 224);
		colors[ImGuiCol_DragDropTarget] = rgb(0, 112, 224);
		colors[ImGuiCol_NavWindowingHighlight] = rgb(0, 112, 224);

		colors[ImGuiCol_PlotHistogram] = rgb(0, 112, 224);
		colors[ImGuiCol_PlotHistogramHovered] = rgb(0, 112, 224);
		colors[ImGuiCol_PlotLines] = rgb(0, 112, 224);
		colors[ImGuiCol_PlotLinesHovered] = rgb(0, 112, 224);

		colors[ImGuiCol_NavWindowingHighlight] = rgb(128, 168, 224);

		colors[ImGuiCol_Border] = grey(21, 0.75f);
		colors[ImGuiCol_PopupBg] = grey(21, 0.9f);
		colors[ImGuiCol_FrameBg] = grey(15, 0.5f);

		colors[ImGuiCol_NavWindowingDimBg] = grey(128, 0.75f);
		colors[ImGuiCol_DockingPreview] = grey(128, 0.25f);
		colors[ImGuiCol_FrameBgHovered] = grey(128, 0.25f);
		colors[ImGuiCol_TextSelectedBg] = grey(128, 0.25f);
		colors[ImGuiCol_TableRowBgAlt] = grey(128, 0.05f);

		colors[ImGuiCol_SliderGrabActive] = grey(128, 0.8f);
		colors[ImGuiCol_ScrollbarGrabActive] = grey(128, 0.8f);
		colors[ImGuiCol_ScrollbarGrabHovered] = grey(128, 0.8f);
		colors[ImGuiCol_ResizeGripActive] = grey(128, 0.8f);
		colors[ImGuiCol_ResizeGripHovered] = grey(128, 0.8f);

		colors[ImGuiCol_SliderGrab] = grey(87, 0.8f);
		colors[ImGuiCol_ScrollbarGrab] = grey(87, 0.8f);
		colors[ImGuiCol_ResizeGrip] = grey(87, 0.8f);

		colors[ImGuiCol_Button] = grey(60, 0.8f);
		colors[ImGuiCol_ButtonHovered] = grey(47, 0.8f);
		colors[ImGuiCol_ButtonActive] = grey(47, 0.8f);

		colors[ImGuiCol_Header] = grey(47, 0.8f);
		colors[ImGuiCol_HeaderHovered] = grey(47, 0.8f);
		colors[ImGuiCol_HeaderActive] = grey(47, 0.8f);

		colors[ImGuiCol_TableHeaderBg] = grey(47, 0.8f);
		colors[ImGuiCol_SeparatorActive] = grey(47, 0.8f);
		colors[ImGuiCol_SeparatorHovered] = grey(47, 0.8f);
		colors[ImGuiCol_TabHovered] = grey(47, 0.8f);

		colors[ImGuiCol_WindowBg] = grey(36, 0.8f);
		colors[ImGuiCol_TabActive] = grey(36, 0.8f);
		colors[ImGuiCol_TabUnfocusedActive] = grey(36, 0.8f);

		colors[ImGuiCol_ChildBg] = grey(28, 0.8f);

		colors[ImGuiCol_Separator] = grey(26, 0.8f);
		colors[ImGuiCol_TableBorderLight] = grey(26, 0.8f);
		colors[ImGuiCol_TableBorderStrong] = grey(26, 0.8f);
		colors[ImGuiCol_ScrollbarBg] = grey(26, 0.8f);

		colors[ImGuiCol_TitleBg] = grey(21, 0.8f);
		colors[ImGuiCol_TitleBgActive] = grey(21, 0.8f);
		colors[ImGuiCol_TitleBgCollapsed] = grey(21, 0.8f);
		colors[ImGuiCol_MenuBarBg] = grey(21, 0.8f);


		style.WindowPadding = ImVec2(4, 4);
		style.FramePadding = ImVec2(6, 6);
		style.ItemSpacing = ImVec2(4, 2);

		style.ScrollbarSize = 12;
		style.ScrollbarRounding = 12;

		style.IndentSpacing = 12;

		style.WindowBorderSize = 1;
		style.ChildBorderSize = 0;
		style.PopupBorderSize = 0;
		style.FrameBorderSize = 0;
		style.PopupRounding = 0;

		style.FrameRounding = 3;
		style.GrabRounding = 3;

		style.WindowRounding = 0;
		style.ChildRounding = 0;
		style.TabBorderSize = 0;
		style.TabRounding = 6.0f;


		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}

	void SetLightThemeColors() {
		ImGui::StyleColorsLight();
	}
}
