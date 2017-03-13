// OpenBBG
#include <OpenBBG/Modules/Module_Game.h>
#include <OpenBBG/Game.h>

namespace openbbg {

IMPLEMENT_MODULE(Module_Game, Phase_Startup, true)
MODULE_DEPENDENCY(Module_Game, Module_Dummy, true)

bool Module_Game::ModuleInit(Game *game)
{
	return true;
}

void Module_Game::ModuleCleanup(Game *game)
{
}

}