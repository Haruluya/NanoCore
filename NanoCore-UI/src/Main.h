#pragma once
#include "log/log.h"
#include "Application.h"

int main(int argc, char** argv)
{
	NanoUI::Log::Init();
	IC_CORE_INFO("LOG INIT!");
	NanoUI::Application* app = new NanoUI::Application();
	app->runWindow();
	delete app;
}
