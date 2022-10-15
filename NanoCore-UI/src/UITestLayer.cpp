
#include "UITestLayer.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ResourcesLoadFactory.hpp"
#include "ImGuizmo/ImGuizmo.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

NanoCore::UITestLayer::UITestLayer()
{
}

void NanoCore::UITestLayer::OnAttach()
{
}

void NanoCore::UITestLayer::OnDetach()
{
}

void NanoCore::UITestLayer::OnUpdate(Timestep ts)
{
	RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	RenderCommand::Clear();
}

void NanoCore::UITestLayer::OnImGuiRender()
{

	// Note: Switch this to true to enable dockspace
	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));


	const float titlebarHeight = 57.0f;
	const ImVec2 windowPadding = ImGui::GetCurrentWindowRead()->WindowPadding;

	ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y));
	const ImVec2 titlebarMin = ImGui::GetCursorScreenPos();
	const ImVec2 titlebarMax = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - windowPadding.y * 2.0f,
								 ImGui::GetCursorScreenPos().y + titlebarHeight };
	auto* drawList = ImGui::GetWindowDrawList();


	{
		int logoWidth, logoHeight;
		GLuint logo_texture = 0;
		if (NanoCore::ResourcesLoadFactory::LoadTextureFromFile("resources/images/haruluya.jpg", &logo_texture, &logoWidth, &logoHeight, 108, 108)) {
			const ImVec2 logoOffset(16.0f + windowPadding.x, 8.0f + windowPadding.y);
			const ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y };
			const ImVec2 logoRectMax = { logoRectStart.x + logoWidth, logoRectStart.y + logoHeight };
			drawList->AddImage((ImTextureID)logo_texture, logoRectStart, logoRectMax);
		}
		else {
			NANO_ENGINE_LOG_ERROR("Logo not found!");
		}
	}
	ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth() - windowPadding.y * 2.0f, ImGui::GetFrameHeightWithSpacing() });

	static float moveOffsetX;
	static float moveOffsetY;
	const float w = ImGui::GetContentRegionAvail().x;
	const float buttonsAreaWidth = 94;

	ImGui::InvisibleButton("##titleBarDragZone", ImVec2(w - buttonsAreaWidth, titlebarHeight));
	m_TitleBarHovered = ImGui::IsItemHovered();

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
		auto* window = static_cast<GLFWwindow*>(NanoCore::Application::Get().GetWindow().GetNativeWindow());
		bool maximized = (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
		if (maximized)
			glfwRestoreWindow(window);
		else
			NanoCore::Application::Get().GetWindow().Maximize();
	}
	else if (ImGui::IsItemActive())
	{
		// TODO: move this stuff to a better place, like Window class
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			auto* window = static_cast<GLFWwindow*>(NanoCore::Application::Get().GetWindow().GetNativeWindow());
			int maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
			if (maximized)
			{
				glfwRestoreWindow(window);

				int newWidth, newHeight;
				glfwGetWindowSize(window, &newWidth, &newHeight);
				if (windowWidth - (float)newWidth > 0.0f)
					moveOffsetX *= (float)newWidth / windowWidth;
			}

			ImVec2 point = ImGui::GetMousePos();
			glfwSetWindowPos(window, point.x - moveOffsetX, point.y - moveOffsetY);

		}
	}

	ImGui::SuspendLayout();
	{
		ImGui::SetItemAllowOverlap();
		const float logoOffset = 16.0f * 2.0f + 41.0f + windowPadding.x;
		ImGui::SetCursorPos(ImVec2(logoOffset, 4.0f));

		if (ImGui::IsItemHovered())
			m_TitleBarHovered = false;
	}

	const float menuBarRight = ImGui::GetItemRectMax().x - ImGui::GetCurrentWindow()->Pos.x;

	// Project name
	{

		const std::string title = "NanoCore-Haruluya";
		const ImVec2 textSize = ImGui::CalcTextSize(title.c_str());
		const float rightOffset = ImGui::GetWindowWidth() / 5.0f;
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - rightOffset - textSize.x);
		ImRect rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		//1466.4,-124,1577.4,32,3,1
		drawList->AddRect({ 1466.4f ,-124.0f }, { 1577.4f,32.0f }, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Border)), 3, 0, 1);
	}

	//ImGui::EndHorizontal();
}

void NanoCore::UITestLayer::OnEvent(Event& e)
{
}
