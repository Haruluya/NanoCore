#include "ncpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

namespace NanoCore{

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		RA_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		RA_PROFILE_FUNCTION();
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		RA_CORE_ASSERT(status, "Failed to initialize Glad!");
		RA_CORE_INFO("Initialize opengl success!");

	}

	void OpenGLContext::SwapBuffers()
	{
		RA_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);
	}

}