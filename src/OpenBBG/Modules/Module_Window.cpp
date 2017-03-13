#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Modules/Module_Window.h>
#include <OpenBBG/Game.h>

// GLFW
#include <glfw/glfw3.h>
#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <glfw/glfw3native.h>

// Windows
#ifdef WIN32
#include <Winuser.h>
#include <ShellScalingAPI.h>
#endif


namespace openbbg {

struct Window
{
	GLFWwindow *glfwWindow;
};


//------------------------------ WIN32 Event Process Middle Man --------------------------------------

#ifdef WIN32
WNDPROC g_glfwWindowProc = nullptr;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window *window = (Window *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (uMsg) {
	case WM_DPICHANGED: {
		WORD xAxis = LOWORD(wParam);
		WORD yAxis = HIWORD(wParam);
		float xScale = (float)xAxis / 96.f;
		float yScale = (float)yAxis / 96.f;
		LOG_DEBUG("DPI Changed: {} {}", xScale, yScale);
	}	break;
	}

	return CallWindowProc(g_glfwWindowProc, hWnd, uMsg, wParam, lParam);
}
#endif


//--------------------------------------------------------------------

IMPLEMENT_MODULE(Module_Window, Phase_Startup, true)
MODULE_DEPENDENCY(Module_Window, Module_Game, true)

bool Module_Window::ModuleInit(Game *game)
{
#ifdef WIN32
	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
#endif

	{
		Window *w = new Window();
		w->glfwWindow = glfwCreateWindow(640, 480, "Test Window", nullptr, nullptr);

#ifdef WIN32
		HWND hWnd = glfwGetWin32Window(w->glfwWindow);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)w);

		// Hijack glfw window process
		g_glfwWindowProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
#endif

		windows.push_back(w);
	}

	return true;
}

void Module_Window::ModuleCleanup(Game *game)
{
	for (auto window : windows)
		delete window;
	windows.clear();
}

bool Module_Window::ShouldClose()
{
	for (auto window : s_moduleInstance->windows)
		if (glfwWindowShouldClose(window->glfwWindow) == false)
			return false;
	return true;
}

void Module_Window::ProcessEvents()
{
	glfwPollEvents();
}


}