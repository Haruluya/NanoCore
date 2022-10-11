#include "ncpch.h"
#include "Window.h"

#include "modules/windows/WinWindow.h"

namespace NanoCore
{
	Unique<Window> Window::Create(const WindowProps& props)
	{
		return std::make_unique<WindowsWindow>(props);
	}

}