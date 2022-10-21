#pragma once

#include "core/base/Base.h"

#include "modules/windows/Window.h"
#include "core/layer/Layer.h"
#include "modules/events/Event.h"
#include "modules/events/ApplicationEvent.h"

#include "modules/utils/Timestep.h"

#include "modules/ui/UILayer.h"

#include <queue>
#include <mutex>
int main(int argc, char** argv);

namespace NanoCore{
	using EventCallbackFn = std::function<void(Event&)>;
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "NanoCoreApplication";
		std::string WorkingDirectory;
		std::string LogoPath;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		template<typename Func>
		void QueueEvent(Func&& func)
		{
			m_EventQueue.push(func);
		}


		template<typename TEvent, bool DispatchImmediately = false, typename... TEventArgs>
		void DispatchEvent(TEventArgs&&... args)
		{
			static_assert(std::is_assignable_v<Event, TEvent>);

			std::shared_ptr<TEvent> event = std::make_shared<TEvent>(std::forward<TEventArgs>(args)...);
			if constexpr (DispatchImmediately)
			{
				OnEvent(*event);
			}
			else
			{
				std::scoped_lock<std::mutex> lock(m_EventQueueMutex);
				m_EventQueue.push([event](){ Application::Get().OnEvent(*event); });
			}
		}


		Window& GetWindow() { return *m_Window; }

		void Close();

		UILayer* GetUILayer() { return m_UILayer; }

		static Application& Get() { return *s_Instance; }

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }
		void SubmitToMainThread(const std::function<void()>& function);
		void Run();
	private:

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		void ProcessEvents();

		void ExecuteMainThreadQueue();
	private:
		ApplicationSpecification m_Specification;
		Unique<Window> m_Window;
		UILayer* m_UILayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;


		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;

		std::mutex m_EventQueueMutex;
		std::queue<std::function<void()>> m_EventQueue;
		std::vector<EventCallbackFn> m_EventCallbacks;



	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in CLIENT
	Application* CreateApplication(ApplicationCommandLineArgs args);

}