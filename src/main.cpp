// OpenBBG
#include <OpenBBG/Module.h>
#include <OpenBBG/Game.h>


namespace openbbg {

int entryFunc(int argc, char *argv[])
{
	Game game;
	Module::PhaseInit(&game, Module::Phase_Startup);
	Module::PhaseCleanup(&game, Module::Phase_Startup);
	return 0;
}


}


int main(int argc, char *argv[])
{
	return openbbg::entryFunc(argc, argv);
}