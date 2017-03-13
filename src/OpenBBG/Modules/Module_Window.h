#pragma once

// OpenBBG
#include <OpenBBG/Module.h>

namespace openbbg {

typedef struct Module_Window Module_Window;
typedef struct Window Window;

struct Module_Window : Module
{
	DECLARE_MODULE(Module_Window)

	Module_Window()
		: Module()
	{
	}
	
	virtual bool ModuleInit(Game *game);
	virtual void ModuleCleanup(Game *game);

	static bool ShouldClose();
	static void ProcessEvents();


	//-------------- Window Event Handlers -------------

	static void HandlePositionChange(Window *window, int x, int y);
	static void HandleSizeChange(Window *window, int x, int y);
	static void HandleClose(Window *window);
	static void HandleRefresh(Window *window);
	static void HandleFocusChange(Window *window, bool hasFocus);
	static void HandleIconifyChange(Window *window, bool isIconified);
	static void HandleFramebufferSizeChange(Window *window, int x, int y);


	//------------------------------------------------



	// TODO: Determine if we should have multiple threads for rendering to each window

	vector<Window *> windows;
};


}