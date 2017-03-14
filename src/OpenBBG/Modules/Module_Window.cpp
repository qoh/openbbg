#include "stdafx.h"

#define TEST_VULKAN 1

// OpenBBG
#include <OpenBBG/Modules/Module_Window.h>
#include <OpenBBG/Window.h>
#include <OpenBBG/Game.h>

// Windows
#ifdef _WIN32
#include <Winuser.h>
#include <ShellScalingAPI.h>
#endif

// TODO: DPI detection for linux


namespace openbbg {

// Window Callbacks

void CallbackClose(GLFWwindow *window)
{
	Module_Window::HandleClose(static_cast<Window *>(glfwGetWindowUserPointer(window)));
}

void CallbackFocusChange(GLFWwindow *window, int state)
{
	Module_Window::HandleFocusChange(static_cast<Window *>(glfwGetWindowUserPointer(window)), state == GLFW_TRUE);
}

void CallbackFramebufferSizeChange(GLFWwindow *window, int x, int y)
{
	Module_Window::HandleFramebufferSizeChange(static_cast<Window *>(glfwGetWindowUserPointer(window)), x, y);
}

void CallbackIconifyChange(GLFWwindow *window, int state)
{
	Module_Window::HandleIconifyChange(static_cast<Window *>(glfwGetWindowUserPointer(window)), state == GLFW_TRUE);
}

void CallbackPositionChange(GLFWwindow *window, int x, int y)
{
	Module_Window::HandlePositionChange(static_cast<Window *>(glfwGetWindowUserPointer(window)), x, y);
}

void CallbackRefresh(GLFWwindow *window)
{
	Module_Window::HandleRefresh(static_cast<Window *>(glfwGetWindowUserPointer(window)));
}

void CallbackSizeChange(GLFWwindow *window, int x, int y)
{
	Module_Window::HandleSizeChange(static_cast<Window *>(glfwGetWindowUserPointer(window)), x, y);
}


// Input Callbacks

void CallbackCharacter(GLFWwindow *window, unsigned int codepoint)
{
	Module_Window::HandleCharacter(static_cast<Window *>(glfwGetWindowUserPointer(window)), codepoint);
}

void CallbackKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	Module_Window::HandleKey(static_cast<Window *>(glfwGetWindowUserPointer(window)), key, scancode, action, mods);
}


//------------------------------ WIN32 Event Process Middle Man --------------------------------------

#ifdef _WIN32
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
#ifdef _WIN32
	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
#endif

	windows.push_back(Window::CreateVulkanWindow(640, 480, "Test Window"));

	return true;
}

void Module_Window::ModuleCleanup(Game *game)
{
	for (auto window : windows)
		delete window;
	windows.clear();
}

void Module_Window::ForceClose()
{
	for (auto window : s_moduleInstance->windows)
		glfwSetWindowShouldClose(window->glfwWindow, GLFW_TRUE);
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


//-------------------------------------------------------------

void Module_Window::SetHandlers(Window *window)
{
#ifdef _WIN32
	HWND hWnd = glfwGetWin32Window(window->glfwWindow);
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);

	// Hijack glfw window process
	if (g_glfwWindowProc == nullptr)
		g_glfwWindowProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
#endif

	glfwSetWindowPosCallback(window->glfwWindow, CallbackPositionChange);
	glfwSetWindowSizeCallback(window->glfwWindow, CallbackSizeChange);
	glfwSetWindowCloseCallback(window->glfwWindow, CallbackClose);
	glfwSetWindowRefreshCallback(window->glfwWindow, CallbackRefresh);
	glfwSetWindowFocusCallback(window->glfwWindow, CallbackFocusChange);
	glfwSetWindowIconifyCallback(window->glfwWindow, CallbackIconifyChange);
	glfwSetFramebufferSizeCallback(window->glfwWindow, CallbackFramebufferSizeChange);
}

// Window Events

void Module_Window::HandleClose(Window *window)
{
	// TODO: Close individual windows, and not whole game
	Game::s_instance->isRunning = false;
	glfwSetWindowShouldClose(window->glfwWindow, GLFW_TRUE);
}

void Module_Window::HandleDPIChange(Window *window, int x, int y)
{
}

void Module_Window::HandleFocusChange(Window *window, bool hasFocus)
{
}

void Module_Window::HandleFramebufferSizeChange(Window *window, int x, int y)
{
}

void Module_Window::HandleIconifyChange(Window *window, bool isIconified)
{
}

void Module_Window::HandlePositionChange(Window *window, int x, int y)
{
}

void Module_Window::HandleRefresh(Window *window)
{
}

void Module_Window::HandleSizeChange(Window *window, int x, int y)
{
}


// Input Events

void Module_Window::HandleCharacter(Window *window, unsigned int codepoint)
{
}

void Module_Window::HandleKey(Window *window, int key, int scancode, int action, int mods)
{
}


}
