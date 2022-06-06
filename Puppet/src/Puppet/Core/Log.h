#pragma once
#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"
namespace Puppet {
	class Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; };
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; };
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}
//Core log
#define PP_CORE_ERROR(...)	::Puppet::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PP_CORE_WARN(...)	::Puppet::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PP_CORE_INFO(...)	::Puppet::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PP_CORE_TRACE(...)	::Puppet::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define PP_CORE_FATAL(...)	::Puppet::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//Client log
#define PP_ERROR(...)	::Puppet::Log::GetClientLogger()->error(__VA_ARGS__)
#define PP_WARN(...)	::Puppet::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PP_INFO(...)	::Puppet::Log::GetClientLogger()->info(__VA_ARGS__)
#define PP_TRACE(...)	::Puppet::Log::GetClientLogger()->trace(__VA_ARGS__)
#define PP_FATAL(...)	::Puppet::Log::GetClientLogger()->fatal(__VA_ARGS__)

