#pragma once

// OpenBBG
#include <OpenBBG/Module.h>

namespace openbbg {

struct Module_Lua : Module
{
	DECLARE_MODULE(Module_Lua)

	Module_Lua()
		: Module()
	{
	}
	
	virtual bool ModuleInit(Game *game);
	virtual void ModuleCleanup(Game *game);

	void ExecuteScript(const char *file);
};


}