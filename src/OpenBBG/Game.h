#pragma once

// OpenBBG
#include <OpenBBG/Job.h>
#include <OpenBBG/Common/Singleton.h>

namespace openbbg {

typedef struct Game Game;

struct Game : Singleton<Game>
{
	volatile bool isRunning;

	Game()
		: isRunning(true)
		, jobsFrameStart { isRunning, false }
		, jobsFrameEnd { isRunning, false }
		, jobsAsync { isRunning, true, std::thread::hardware_concurrency() }
	{
	}

	JobPool jobsFrameStart;

	JobPool jobsFrameEnd;

	JobPool jobsAsync;
};


}