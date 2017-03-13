#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Job.h>
#include <OpenBBG/Game.h>

namespace openbbg {

void JobPool::WorkerFunc(Game *game, JobPool *pool)
{
	while (game->isRunning) {
		function<void()> job;
		{
			std::unique_lock<std::mutex> lk(pool->m);
			pool->cv.wait(lk, [pool, game]{ return game->isRunning == false || pool->jobs.empty() == false; });
			if (game->isRunning == false)
				break;
			job = pool->jobs.front();
			pool->jobs.pop_front();
			if (pool->jobs.empty() == false)
				pool->cv.notify_one();
		}
		job();
	}
}

}