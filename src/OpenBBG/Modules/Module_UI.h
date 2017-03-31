#pragma once

// OpenBBG
#include <OpenBBG/Module.h>

namespace openbbg {
	
typedef struct Module_UI Module_UI;

struct Module_UI : Module
{
	DECLARE_MODULE(Module_UI)

	Module_UI()
		: Module()
	{
	}
	
	virtual bool ModuleInit(Game *game);
	virtual void ModuleCleanup(Game *game);
};


}