#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Modules/Module_Lua.h>
#include <OpenBBG/Modules/Module_UI.h>
#include <OpenBBG/Game.h>
#include <OpenBBG/UI/UI_Context.h>
#include <OpenBBG/UI/UI_Control.h>

namespace openbbg {

IMPLEMENT_MODULE(Module_UI, Phase_Startup, true)
MODULE_DEPENDENCY(Module_UI, Module_Lua, true)

bool Module_UI::ModuleInit(Game *game)
{
	// Test UI
#if 1
	UI_Context *ctx = new UI_Context();
	UI_Control *root = new UI_Control();
	ctx->SetRoot(root);
#endif
	Module_Lua::Get()->ExecuteScript("assets/scripts/ui-test.lua");

	return true;
}

void Module_UI::ModuleCleanup(Game *game)
{
	UI_Context::CleanupAll();
}

}
