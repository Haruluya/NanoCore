#pragma once


/*
	»’÷æ≈‰÷√°£
*/

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace NanoUI {

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define IC_CORE_TICCE(...)    ::NanoUI::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define IC_CORE_INFO(...)     ::NanoUI::Log::GetCoreLogger()->info(__VA_ARGS__)
#define IC_CORE_WARN(...)     ::NanoUI::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define IC_CORE_ERROR(...)    ::NanoUI::Log::GetCoreLogger()->error(__VA_ARGS__)
#define IC_CORE_FATAL(...)    ::NanoUI::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define IC_TICCE(...)	      ::NanoUI::Log::GetClientLogger()->trace(__VA_ARGS__)
#define IC_INFO(...)	      ::NanoUI::Log::GetClientLogger()->info(__VA_ARGS__)
#define IC_WARN(...)	      ::NanoUI::Log::GetClientLogger()->warn(__VA_ARGS__)
#define IC_ERROR(...)	      ::NanoUI::Log::GetClientLogger()->error(__VA_ARGS__)
#define IC_FATAL(...)	      ::NanoUI::Log::GetClientLogger()->fatal(__VA_ARGS__)

#define IC_ASSERT(x, ...) { if(!(x)) { IC_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define IC_CORE_ASSERT(x, ...) { if(!(x)) { IC_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }