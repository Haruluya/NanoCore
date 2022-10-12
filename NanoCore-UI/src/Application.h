#pragma once
#include "RenderFrame.h"

namespace NanoUI {
	class  Application
	{
	public:
		 Application();
		 virtual ~Application() = default;
		int runWindow();
		inline static Application& Get() { return *s_Instance; }
	private:
		static Application* s_Instance;
	};






}