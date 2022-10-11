#include <NanoCore.h>
#include <Main.h>


#include "EditorLayer.h"

namespace NanoCore {

	class EditorApp : public Application
	{
	public:
		EditorApp(const ApplicationSpecification& spec)
			: Application(spec)
		{
			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = u8"NanoCore-Engine";
		spec.CommandLineArgs = args;

		return new EditorApp(spec);
	}

}