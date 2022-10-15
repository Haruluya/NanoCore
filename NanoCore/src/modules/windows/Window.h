#pragma once


#include "modules/ref/Shared.h"

#include "modules/events/Event.h"

namespace NanoCore{

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "NanoCore",
			uint32_t width = 1080,
			uint32_t height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};


	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;
		virtual void Maximize() = 0;
		virtual void* GetNativeWindow() const = 0;
		static Unique<Window> Create(const WindowProps& props = WindowProps());

	};

}