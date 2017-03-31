#pragma once

// OpenBBG
#include <OpenBBG/Job.h>

namespace openbbg {

typedef struct Game Game;

struct Game
{
	static Game *s_instance;

	static inline Game *Get()
	{
		return s_instance;
	}

	volatile bool isRunning;

	Game()
		: isRunning(true)
		, jobsFrameStart { isRunning, false }
		, jobsFrameEnd { isRunning, false }
		, jobsAsync { isRunning, true, std::thread::hardware_concurrency() }
	{
		s_instance = this;
	}

	JobPool jobsFrameStart;

	JobPool jobsFrameEnd;

	JobPool jobsAsync;
};


}