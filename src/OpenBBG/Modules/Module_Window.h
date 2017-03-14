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
	static void ForceClose();
	static void ProcessEvents();


	//-------------- Event Handlers -------------
	
	// Window Events
	// REF: http://www.glfw.org/docs/latest/group__window.html
	static void HandleClose(Window *window);
	static void HandleDPIChange(Window *window, int x, int y);
	static void HandleFocusChange(Window *window, bool hasFocus);
	static void HandleFramebufferSizeChange(Window *window, int x, int y);
	static void HandleIconifyChange(Window *window, bool isIconified);
	static void HandlePositionChange(Window *window, int x, int y);
	static void HandleRefresh(Window *window);
	static void HandleSizeChange(Window *window, int x, int y);

	// Input Events
	// REF: http://www.glfw.org/docs/latest/group__input.html
	static void HandleCharacter(Window *window, unsigned int codepoint);
	static void HandleCursorEnter(Window *window, bool isCursorIn);
	static void HandleCursorPositionChange(Window *window, double x, double y);
	static void HandleKey(Window *window, int key, int scancode, int action, int mods);
	static void HandleMouseButton(Window *window, int button, int action, int mods);
	static void HandleScroll(Window *window, double x, double y);
	static void HandleFileDrop(Window *window, int count, const char **files);

	static void SetHandlers(Window *window);


	//------------------------------------------------



	// TODO: Determine if we should have multiple threads for rendering to each window

	vector<Window *> windows;
};


}