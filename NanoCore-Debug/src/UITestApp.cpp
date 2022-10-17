#include <NanoCore.h>
#include <Main.h>


#include "UITestLayer.h"

namespace NanoCore {

	class UITestApp : public Application
	{
	public:
		UITestApp(const ApplicationSpecification& spec)
			: Application(spec)
		{
			PushLayer(new UITestLayer());
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = u8"NanoCore-Engine";
		spec.CommandLineArgs = args;



		return new UITestApp(spec);
	}

}