#pragma once

// OpenBBG
#include <OpenBBG/Module.h>

namespace openbbg {
	
typedef struct Module_Game Module_Game;

struct Module_Game : Module
{
	DECLARE_MODULE(Module_Game)

	Module_Game()
		: Module()
	{
	}
	
	virtual bool ModuleInit(Game *game);
	virtual void ModuleCleanup(Game *game);
};


}