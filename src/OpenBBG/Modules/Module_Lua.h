#pragma once

// OpenBBG
#include <OpenBBG/Module.h>

#define LUA_BINDING_API __declspec(dllexport)

namespace openbbg {
	
typedef struct Module_Lua Module_Lua;

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