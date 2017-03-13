// OpenBBG
#include <OpenBBG/Modules/Module_Dummy.h>
#include <OpenBBG/Game.h>

namespace openbbg {

IMPLEMENT_MODULE(Module_Dummy, Phase_Startup, true)

bool Module_Dummy::ModuleInit(Game *game)
{
	return true;
}

void Module_Dummy::ModuleCleanup(Game *game)
{
}

}