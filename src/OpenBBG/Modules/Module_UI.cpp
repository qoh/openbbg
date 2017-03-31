#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Modules/Module_UI.h>
#include <OpenBBG/Game.h>
#include <OpenBBG/UI/UI_Control.h>

namespace openbbg {

IMPLEMENT_MODULE(Module_UI, Phase_Startup, true)
MODULE_DEPENDENCY(Module_UI, Module_Lua, true)

bool Module_UI::ModuleInit(Game *game)
{
	return true;
}

void Module_UI::ModuleCleanup(Game *game)
{
}

}
