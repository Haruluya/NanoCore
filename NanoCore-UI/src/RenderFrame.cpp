#include "ICpch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ResourcesLoadFactory.hpp"
#include "RenderFrame.h"


#include <imgui/imgui.h>
#include <Imgui/imgui_internal.h>


#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

namespace NanoUI {

    static void glfw_error_callback(int error, const char* description)
    {
        IC_CORE_ERROR("Glfw Error %d: %s\n", error, description);
    }

    GLFWwindow* RenderFrame::Create_glfw_Window(int width,int height,const char* title)
    {
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) {
            IC_CORE_ERROR("glfw init failed!");
            return NULL;
        }
#if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
        const char* glsl_version = "#version 100";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
        // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (window == NULL)
        {
            IC_CORE_ERROR("window init failed!");
            return NULL;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            IC_CORE_ERROR("Failed to initialize GLAD");
            return NULL;
        }

        NanoCore::ResourcesLoadFactory::Init();

        return window;
    }

    void RenderFrame::RenderMainImGui()
    {
		const float titlebarHeight = 57.0f;
		const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;


		ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y));
		const ImVec2 titlebarMin = ImGui::GetCursorScreenPos();
		const ImVec2 titlebarMax = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - windowPadding.y * 2.0f,
									 ImGui::GetCursorScreenPos().y + titlebarHeight };
		auto* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(titlebarMin, titlebarMax, IM_COL32(21, 21, 21, 255));

		// Logo
		{
			int my_image_width;
			int my_image_height;
			GLuint my_image_texture = 0;
			bool ret = NanoCore::ResourcesLoadFactory::LoadTextureFromFile("resources/images/haruluya.jpg", &my_image_texture, &my_image_width, &my_image_height, 256, 256);

			const int logoWidth = my_image_width;
			const int logoHeight = my_image_height;
			const ImVec2 logoOffset(16.0f + windowPadding.x, 8.0f + windowPadding.y);
			const ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y };
			const ImVec2 logoRectMax = { logoRectStart.x + logoWidth, logoRectStart.y + logoHeight };
			drawList->AddImage((ImTextureID)my_image_texture, logoRectStart, logoRectMax);
		}



    }
}