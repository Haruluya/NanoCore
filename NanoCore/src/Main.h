#pragma once
extern NanoCore::Application* NanoCore::CreateApplication(ApplicationCommandLineArgs);



int main(int argc, char** argv)
{
	// 初始化日志配置。
	NanoCore::Log::Init();
	RA_PROFILE_BEGIN_SESSION("Startup", "Profile-Startup.json");
	auto app = NanoCore::CreateApplication({argc,argv});
	RA_PROFILE_END_SESSION();

	RA_PROFILE_BEGIN_SESSION("Runtime", "Profile-Runtime.json");
	app->Run();
	RA_PROFILE_END_SESSION();

	RA_PROFILE_BEGIN_SESSION("Startup", "Profile-Shutdown.json");
	delete app;
	RA_PROFILE_END_SESSION();
}