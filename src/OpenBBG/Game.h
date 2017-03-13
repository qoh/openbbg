#pragma once

namespace openbbg {

typedef struct Game Game;

struct Game
{
	volatile bool isRunning;

	Game()
		: isRunning(true)
	{
	}
};

}