#pragma once

// spdlog
#include <spdlog/spdlog.h>

namespace openbbg {

typedef struct Log Log;

struct Log
{
	static shared_ptr<spdlog::logger> s_logMain;
	static shared_ptr<spdlog::logger> s_logOutput;
	static shared_ptr<spdlog::logger> s_logError;

	static bool Init();
	static void Cleanup();
};

#define LOG_INFO(...) Log::s_logMain->info(__VA_ARGS__); Log::s_logOutput->info(__VA_ARGS__);
#define LOG_DEBUG(...) Log::s_logMain->debug(__VA_ARGS__); Log::s_logOutput->debug(__VA_ARGS__);
#define LOG_ERROR(...) Log::s_logMain->error(__VA_ARGS__); Log::s_logError->error(__VA_ARGS__);
#define LOG_WARN(...) Log::s_logMain->warn(__VA_ARGS__); Log::s_logOutput->warn(__VA_ARGS__);

}