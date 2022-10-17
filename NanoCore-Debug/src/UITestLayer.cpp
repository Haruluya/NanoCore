
#include "UITestLayer.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ResourcesLoadFactory.hpp"
#include "ImGuizmo/ImGuizmo.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "utils/ImGuiUtils.hpp"

NanoCore::UITestLayer::UITestLayer()
{
	ResourcesLoadFactory::Init();
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

	/*
		Set main layout.
	*/
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	auto boldFont = io.Fonts->Fonts[0];
	auto largeFont = io.Fonts->Fonts[1];


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
	ImGui::Begin("DockSpace Demo", nullptr, window_flags);
	ImGui::PopStyleColor(); // MenuBarBg
	ImGui::PopStyleVar(2);

	ImGui::PopStyleVar(2);
	const float titlebarHeight = 57.0f;
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
		const ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y };
		const ImVec2 logoRectMax = { logoRectStart.x + logoWidth, logoRectStart.y + logoHeight };
		drawList->AddImage((ImTextureID)ResourcesLoadFactory::LogoTexture->GetRendererID(), logoRectStart, logoRectMax);
	}

	ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth() - windowPadding.y * 2.0f, ImGui::GetFrameHeightWithSpacing() });

	static float moveOffsetX;
	static float moveOffsetY;
	const float w = ImGui::GetContentRegionAvail().x;
	const float buttonsAreaWidth = 94;

	// Title bar drag area

	// On Windows we hook into the GLFW win32 window internals
#ifdef HZ_PLATFORM_WINDOWS

	ImGui::InvisibleButton("##titleBarDragZone", ImVec2(w - buttonsAreaWidth, titlebarHeight));
	m_TitleBarHovered = ImGui::IsItemHovered() && (Input::GetCursorMode() != CursorMode::Locked);

#else
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
#endif

	// Draw Menubar

	ImGui::SuspendLayout();
	{
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

				if (ImGui::BeginMenu("File"))
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

	const float menuBarRight = ImGui::GetItemRectMax().x - ImGui::GetCurrentWindow()->Pos.x;

	// Project name
	{

		UI::ScopedColour textColour(ImGuiCol_Text, UI::Color::textDarker);
		UI::ScopedColour border(ImGuiCol_Border, IM_COL32(40, 40, 40, 255));

		const std::string title = Project::GetActive()->GetConfig().Name;
		const ImVec2 textSize = ImGui::CalcTextSize(title.c_str());
		const float rightOffset = ImGui::GetWindowWidth() / 5.0f;
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - rightOffset - textSize.x);
		UI::ShiftCursorY(1.0f + windowPadding.y);

		{
			UI::ScopedFont boldFont(ImGui::GetIO().Fonts->Fonts[0]);
			ImGui::Text(title.c_str());
		}
		UI::SetTooltip("Current project (" + Project::GetActive()->GetConfig().ProjectFileName + ")");


		UI::DrawBorder(UI::RectExpanded(UI::GetItemRect(), 24.0f, 68.0f), 1.0f, 3.0f, 0.0f, -60.0f);
	}

	// Current Scene name
	{
		UI::ScopedColour textColour(ImGuiCol_Text,UI::Color::text);
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
			// TODO: move this stuff to a better place, like Window class
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
			// TODO: move this stuff to a better place, like Window class
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

void NanoCore::UITestLayer::OnEvent(Event& e)
{
}
