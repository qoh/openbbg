#pragma once

// OpenBBG
#include <OpenBBG/Module.h>

namespace openbbg {

struct Module_Dummy : Module
{
	DECLARE_MODULE(Module_Dummy)

	Module_Dummy()
		: Module()
	{
	}
	
	virtual bool ModuleInit(Game *game);
	virtual void ModuleCleanup(Game *game);
};


}