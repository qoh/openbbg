#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Module.h>
#include <OpenBBG/Game.h>
#include <OpenBBG/Job.h>
#include <OpenBBG/Modules/Module_Window.h>

#include <GLFW/glfw3.h>

#define TEST 0

struct test
{
	int a;

	test()
	{
//		printf("Create\n");
	}

	~test()
	{
//		printf("Kill: %d\n", a);
	}
};


namespace openbbg {

int entryFunc(int argc, char *argv[])
{
	Log::Init();
	glfwInit();

	Game game;

	// Init
	Module::PhaseInit(&game, Module::Phase_Startup);

	// Main Loop
	{
#if TEST
	test asdf;
	asdf.a = 25;

	std::shared_ptr<test> b(new test);
	b->a = 50;

	double startTime = glfwGetTime();
	
#if 0
	JobPool testPool(&game, false);

	for (uint32_t a = 0; a < 3000; ++a)
	testPool.Queue([&testPool, asdf, b]() {
		LOG_INFO("Job: {} {}", asdf.a, b->a);

		testPool.Queue([&testPool]() {
			LOG_INFO("Embedded Job");
			
			testPool.Queue([&testPool]() {
				LOG_INFO("Embedded Job 2");
			});
		});
	});

	testPool.ProcessAllCurrent();
	testPool.ProcessAllCurrent();
	testPool.ProcessAllCurrent();
#else
	unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
	JobPool testPool(true, game.isRunning, concurentThreadsSupported == 0 ? 8 : concurentThreadsSupported);

	for (uint32_t a = 0; a < 3000; ++a)
	testPool.Queue([&testPool, asdf, b]() {
		LOG_INFO("Job: {} {}", asdf.a, b->a);
		testPool.Queue([&testPool]() {
			LOG_INFO("Embedded Job");
			testPool.Queue([&testPool]() {
				LOG_INFO("Embedded Job 2");
			});
		});
	});
#endif
#endif

	while (Module_Window::ShouldClose() == false) {
		Module_Window::ProcessEvents();
	}


	// Cleanup
	game.isRunning = false;

	Module::PhaseCleanup(&game, Module::Phase_Startup);
	}
#if TEST
	double finTime = glfwGetTime();
	LOG_INFO("Elapsed Time: {}", finTime - startTime);
#endif

	Log::Cleanup();
	return 0;
}


}


int main(int argc, char *argv[])
{
	return openbbg::entryFunc(argc, argv);
}