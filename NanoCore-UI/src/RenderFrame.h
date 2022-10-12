#pragma once
#include <GLFW/glfw3.h> 


namespace NanoUI {
	class RenderFrame {
	public:
		static GLFWwindow* Create_glfw_Window(int, int, const char*);
		static void RenderMainImGui();
	};
}