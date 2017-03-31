#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Module.h>
#include <OpenBBG/Game.h>
#include <OpenBBG/Job.h>
#include <OpenBBG/Modules/Module_Window.h>

// GLFW
#include <GLFW/glfw3.h>

namespace openbbg {

int entryFunc(int argc, char *argv[])
{
	// Init
	Log::Init();
	glfwInit();
	Game game;
	Module::PhaseInit(&game, Module::Phase_Startup);

	// Main Loop
	while (Module_Window::ShouldClose() == false)
		Module_Window::ProcessEvents();

	// Cleanup
	game.isRunning = false;
	Module::PhaseCleanup(&game, Module::Phase_Startup);
	Log::Cleanup();

	return 0;
}


}


int main(int argc, char *argv[])
{
	return openbbg::entryFunc(argc, argv);
}