#pragma once

// OpenBBG
#include <OpenBBG/Module.h>

namespace openbbg {

struct Module_Window : Module
{
	DECLARE_MODULE(Module_Window)

	Module_Window()
		: Module()
	{
	}
	
	virtual bool ModuleInit(Game *game);
	virtual void ModuleCleanup(Game *game);
};


}