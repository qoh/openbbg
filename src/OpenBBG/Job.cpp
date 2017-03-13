#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Job.h>

namespace openbbg {

void JobPool::WorkerFunc(JobPool *pool)
{
#if 1
	while (true) {
		function<void()> job;
		{
			std::unique_lock<std::mutex> lk(pool->m);
			pool->cv.wait(lk, [pool]{ return pool->keepAlive == false || pool->jobs.empty() == false; });
			if (pool->keepAlive == false && pool->jobs.empty()) {
				pool->cv.notify_all();
				break;
			}
			job = pool->jobs.front();
			pool->jobs.pop_front();
			if (pool->jobs.empty() == false)
				pool->cv.notify_one();
		}
		job();
	}
#else
	while (pool->keepAlive) {
		function<void()> job;
		{
			std::unique_lock<std::mutex> lk(pool->m);
			pool->cv.wait(lk, [pool]{ return pool->keepAlive == false || pool->jobs.empty() == false; });
			// Stop processing jobs if app is closing
			if (pool->keepAlive == false)
				break;
			job = pool->jobs.front();
			pool->jobs.pop_front();
			if (pool->jobs.empty() == false)
				pool->cv.notify_one();
		}
		job();
	}
#endif
}

}