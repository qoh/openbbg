#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Modules/Module_Window.h>
#include <OpenBBG/Game.h>

namespace openbbg {

IMPLEMENT_MODULE(Module_Window, Phase_Startup, true)
MODULE_DEPENDENCY(Module_Window, Module_Game, true)

bool Module_Window::ModuleInit(Game *game)
{
	return true;
}

void Module_Window::ModuleCleanup(Game *game)
{
}

}