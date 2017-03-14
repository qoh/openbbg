#pragma once

namespace openbbg {

typedef struct Game Game;

struct Game
{
	static Game *s_instance;

	volatile bool isRunning;

	Game()
		: isRunning(true)
	{
		s_instance = this;
	}
};


}