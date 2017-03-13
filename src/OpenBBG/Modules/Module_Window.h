#pragma once

// OpenBBG
#include <OpenBBG/Module.h>

namespace openbbg {

typedef struct Module_Window Module_Window;
typedef struct Window Window;

struct Module_Window : Module
{
	DECLARE_MODULE(Module_Window)

	Module_Window()
		: Module()
	{
	}
	
	virtual bool ModuleInit(Game *game);
	virtual void ModuleCleanup(Game *game);

	static bool ShouldClose();
	static void ProcessEvents();

	// TODO: Determine if we should have multiple threads for rendering to each window

	vector<Window *> windows;
};


}