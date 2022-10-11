#pragma once


/*
	»’÷æ≈‰÷√°£
*/



#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace NanoCore{

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
#define RA_CORE_TRACE(...)    ::NanoCore::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RA_CORE_INFO(...)     ::NanoCore::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RA_CORE_WARN(...)     ::NanoCore::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RA_CORE_ERROR(...)    ::NanoCore::Log::GetCoreLogger()->error(__VA_ARGS__)
#define RA_CORE_FATAL(...)    ::NanoCore::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#define RA_CORE_CRITICAL(...) ::NanoCore::Log::GetClientLogger()->critical(__VA_ARGS__)


// Client log macros
#define RA_TRACE(...)	      ::NanoCore::Log::GetClientLogger()->trace(__VA_ARGS__)
#define RA_INFO(...)	      ::NanoCore::Log::GetClientLogger()->info(__VA_ARGS__)
#define RA_WARN(...)	      ::NanoCore::Log::GetClientLogger()->warn(__VA_ARGS__)
#define RA_ERROR(...)	      ::NanoCore::Log::GetClientLogger()->error(__VA_ARGS__)
#define RA_FATAL(...)	      ::NanoCore::Log::GetClientLogger()->fatal(__VA_ARGS__)
#define RA_CRITICAL(...)      ::NanoCore::Log::GetClientLogger()->critical(__VA_ARGS__)


#define BIT(x) (1 << x)

#define RA_ASSERT(x, ...) { if(!(x)) { RA_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define RA_CORE_ASSERT(x, ...) { if(!(x)) { RA_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define RA_CORE_ASSERT(x) { if(!(x)) { RA_CORE_ERROR("Assertion Failed!"); __debugbreak(); } }
#define RA_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)