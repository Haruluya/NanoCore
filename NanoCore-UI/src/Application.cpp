#include "ICpch.h"
#include"Application.h"
#include "Main.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
namespace NanoUI{

    Application* Application::s_Instance = nullptr;
    Application::Application()
    {
        IC_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;
    }
    int Application::runWindow()
    {


#if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
        const char* glsl_version = "#version 100";
#elif defined(__APPLE__)
        // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
#else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
#endif
        // Create window with graphics context,
        GLFWwindow* window = RenderFrame::Create_glfw_Window(1200, 900, "ImguiTesting");


        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // 启动Docking。
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // 启动多窗口。   
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // font.
        io.Fonts->AddFontFromFileTTF("resource/font/NotoSansCJKjp-Medium.otf", 20.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

        // Setup Dear ImGui style
        //ImGui::StyleColorsDark();
        ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Main loop
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();



            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();


         
            RenderFrame::RenderMainImGui();





   
            // Rendering
            ImGui::Render();
            // 多窗口。
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }


            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            glfwSwapBuffers(window);
        }

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
    }

}