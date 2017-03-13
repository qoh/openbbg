#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Log.h>

// Standard Library
#include <cstdarg>

// spdlog
#include <spdlog/sinks/dist_sink.h>
#ifdef _WIN32
#include <spdlog/sinks/msvc_sink.h>
#endif

namespace openbbg {

shared_ptr<spdlog::logger> Log::s_logOutput;
shared_ptr<spdlog::logger> Log::s_logError;

bool Log::Init()
{
	const char *logFormat = "[%Y-%m-%d %T.%e] [%l] %v";

#ifdef NDEBUG
	spdlog::set_level(spdlog::level::info);
#else
	spdlog::set_level(spdlog::level::debug);
#endif

	// Output Logger
	{
		auto distSink = make_shared<spdlog::sinks::dist_sink_mt>();
	
		// stdout
		distSink->add_sink(make_shared<spdlog::sinks::stdout_sink_mt>());

		// MSVC Debugger Console
#ifdef _WIN32
		if (IsDebuggerPresent())
			distSink->add_sink(make_shared<spdlog::sinks::msvc_sink_mt>());
#endif

		s_logOutput = make_shared<spdlog::logger>("output", distSink);
		spdlog::register_logger(s_logOutput);
		s_logOutput->set_pattern(logFormat);
		s_logOutput->flush_on(spdlog::level::debug);
	}

	// Error Logger
	s_logError = spdlog::stderr_logger_mt("error");
	s_logError->set_pattern(logFormat);
	s_logError->flush_on(spdlog::level::debug);

	return true;
}

void Log::Cleanup()
{
	s_logOutput.reset();
	s_logError.reset();
}

}