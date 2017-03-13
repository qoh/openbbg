#pragma once

// OpenBBG
#include <OpenBBG/Config.h>

namespace openbbg {

typedef struct JobPool JobPool;
typedef struct Game Game;

struct JobPool
{
	bool isAsync;
	std::mutex m;
	std::condition_variable cv;
	deque<function<void()>> jobs;
	vector<std::thread> threads;

	JobPool(Game *game, bool isAsync, uint16_t numThreads = 1)
		: isAsync(isAsync)
	{
		if (isAsync) {
			threads.reserve(numThreads);
			for (uint16_t a = 0; a < numThreads; ++a)
				threads.push_back(std::thread(WorkerFunc, game, this));
		}
	}

	~JobPool()
	{
		if (isAsync) {
			cv.notify_all();
			for (auto &t : threads)
				t.join();
			threads.clear();
		}
	}

	inline void Queue(function<void()> job)
	{
		std::lock_guard<std::mutex> lk(m);
		jobs.push_back(job);
		if (isAsync)
			cv.notify_one();
	}

	inline void ProcessAllCurrent()
	{
		deque<function<void()>> jobList;
		{
			std::lock_guard<std::mutex> lk(m);
			jobList = jobs;
		}
		for (auto &job : jobList)
			job();
	}

	static void WorkerFunc(Game *game, JobPool *pool);
};


}