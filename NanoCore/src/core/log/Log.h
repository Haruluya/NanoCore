#pragma once


/*
	»’÷æ≈‰÷√°£
*/



#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "LogMacros.h"

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



