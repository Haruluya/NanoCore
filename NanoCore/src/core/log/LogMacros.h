// Core log macros
#define NANO_ENGINE_LOG_TRANCE(...)    ::NanoCore::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define NANO_ENGINE_LOG_INFO(...)     ::NanoCore::Log::GetCoreLogger()->info(__VA_ARGS__)
#define NANO_ENGINE_LOG_WARN(...)     ::NanoCore::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define NANO_ENGINE_LOG_ERROR(...)    ::NanoCore::Log::GetCoreLogger()->error(__VA_ARGS__)
#define NANO_ENGINE_LOG_FATAL(...)    ::NanoCore::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#define NANO_ENGINE_LOG_CRITICAL(...) ::NanoCore::Log::GetClientLogger()->critical(__VA_ARGS__)
#define NANO_ENGINE_LOG_ASSERT(x, ...) { if(!(x)) { NANO_ENGINE_LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define NANO_ENGINE_LOG_ASSERT(x) { if(!(x)) { NANO_ENGINE_LOG_ERROR("Assertion Failed!"); __debugbreak(); } }



// Client log macros
#define NANO_APP_LOG_TRACE(...)	      ::NanoCore::Log::GetClientLogger()->trace(__VA_ARGS__)
#define NANO_APP_LOG_INFO(...)	      ::NanoCore::Log::GetClientLogger()->info(__VA_ARGS__)
#define NANO_APP_LOG_WARN(...)	      ::NanoCore::Log::GetClientLogger()->warn(__VA_ARGS__)
#define NANO_APP_LOG_ERROR(...)	      ::NanoCore::Log::GetClientLogger()->error(__VA_ARGS__)
#define NANO_APP_LOG_FATAL(...)	      ::NanoCore::Log::GetClientLogger()->fatal(__VA_ARGS__)
#define NANO_APP_LOG_CRITICAL(...)      ::NanoCore::Log::GetClientLogger()->critical(__VA_ARGS__)
#define NANO_APP_LOG_ASSERT(x, ...) { if(!(x)) { NANO_APP_LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }