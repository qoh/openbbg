#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Log.h>
#include <OpenBBG/Modules/Module_Window.h>

// Standard Library
#include <cstdarg>
#include <signal.h>

// spdlog
#include <spdlog/sinks/dist_sink.h>
#ifdef _WIN32
#include <spdlog/sinks/msvc_sink.h>
#endif

namespace openbbg {

shared_ptr<spdlog::logger> Log::s_logMain;
shared_ptr<spdlog::logger> Log::s_logOutput;
shared_ptr<spdlog::logger> Log::s_logError;

extern "C" void signalHandler(int sig)
{
	if (Game::s_instance != nullptr && Game::s_instance->isRunning) {
		Game::s_instance->isRunning = false;
		Module_Window::ForceClose();
	}
}

bool Log::Init()
{
	const char *logFormat = "[%Y-%m-%d %T.%e] [%l] %v";
	const char *logFile = "console.log";

	// Main Logger
	{
		auto distSink = make_shared<spdlog::sinks::dist_sink_mt>();
	
		// MSVC Debugger Console
#ifdef _WIN32
		if (IsDebuggerPresent())
			distSink->add_sink(make_shared<spdlog::sinks::msvc_sink_mt>());
#endif
		// Log File
		distSink->add_sink(make_shared<spdlog::sinks::simple_file_sink_mt>(logFile, true));

		s_logMain = make_shared<spdlog::logger>("main", distSink);
		spdlog::register_logger(s_logMain);
		s_logMain->set_pattern(logFormat);
		s_logMain->flush_on(spdlog::level::debug);
	}

	// Output Logger
	s_logOutput = spdlog::stdout_logger_mt("output");
	s_logOutput->set_pattern(logFormat);
	s_logOutput->flush_on(spdlog::level::debug);

	// Error Logger
	s_logError = spdlog::stderr_logger_mt("error");
	s_logError->set_pattern(logFormat);
	s_logError->flush_on(spdlog::level::debug);

	// Set log level (must come after log initialization)
#ifdef NDEBUG
	spdlog::set_level(spdlog::level::info);
#else
	spdlog::set_level(spdlog::level::debug);
#endif

	// Set signal handlers
	signal(SIGABRT, &signalHandler);
	signal(SIGINT, &signalHandler);
	signal(SIGTERM, &signalHandler);

	return true;
}

void Log::Cleanup()
{
	s_logOutput.reset();
	s_logError.reset();
}

}
