#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Module.h>
#include <OpenBBG/Game.h>
#include <OpenBBG/Job.h>

#include <GLFW/glfw3.h>

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
	glfwInit();

	Game game;

	test asdf;
	asdf.a = 25;

	std::shared_ptr<test> b(new test);
	b->a = 50;

	double startTime = glfwGetTime();

#if 0
	JobPool testPool(&game, false);

	for (uint32_t a = 0; a < 3000; ++a)
	testPool.Queue([&testPool, asdf, b]() {
		printf("Job: %d %d\n", asdf.a, b->a);

		testPool.Queue([&testPool]() {
			printf("Embedded Job\n");
			
			testPool.Queue([&testPool]() {
				printf("Embedded Job 2\n");
			});
		});
	});

	testPool.ProcessAllCurrent();
	testPool.ProcessAllCurrent();
	testPool.ProcessAllCurrent();
#else
	JobPool testPool(&game, true, 10);

	for (uint32_t a = 0; a < 3000; ++a)
	testPool.Queue([&testPool, asdf, b]() {
//		printf("Job: %d %d\n", asdf.a, b->a);

		testPool.Queue([&testPool]() {
//			printf("Embedded Job\n");
			
			testPool.Queue([&testPool]() {
//				printf("Embedded Job 2\n");
			});
		});
	});
#endif

	// Init
	Module::PhaseInit(&game, Module::Phase_Startup);

	// Main Loop

	// Cleanup
	game.isRunning = false;
	Module::PhaseCleanup(&game, Module::Phase_Startup);
	
	double finTime = glfwGetTime();

	printf("Elapsed Time: %f\n", finTime - startTime);

	return 0;
}


}


int main(int argc, char *argv[])
{
	return openbbg::entryFunc(argc, argv);
}