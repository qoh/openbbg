#include "stdafx.h"

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
	Module_Window::HandleFocusChange(static_cast<Window *>(glfwGetWindowUserPointer(window)), GLFW_TRUE == state);
}

void CallbackFramebufferSizeChange(GLFWwindow *window, int x, int y)
{
	Module_Window::HandleFramebufferSizeChange(static_cast<Window *>(glfwGetWindowUserPointer(window)), x, y);
}

void CallbackIconifyChange(GLFWwindow *window, int state)
{
	Module_Window::HandleIconifyChange(static_cast<Window *>(glfwGetWindowUserPointer(window)), GLFW_TRUE == state);
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

void CallbackCursorEnter(GLFWwindow *window, int entered)
{
	Module_Window::HandleCursorEnter(static_cast<Window *>(glfwGetWindowUserPointer(window)), GLFW_TRUE == entered);
}

void CallbackCursorPositionChange(GLFWwindow *window, double x, double y)
{
	Module_Window::HandleCursorPositionChange(static_cast<Window *>(glfwGetWindowUserPointer(window)), x, y);
}

void CallbackKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	Module_Window::HandleKey(static_cast<Window *>(glfwGetWindowUserPointer(window)), key, scancode, action, mods);
}

void CallbackMouseButton(GLFWwindow *window, int button, int action, int mods)
{
	Module_Window::HandleMouseButton(static_cast<Window *>(glfwGetWindowUserPointer(window)), button, action, mods);
}

void CallbackScroll(GLFWwindow *window, double x, double y)
{
	Module_Window::HandleScroll(static_cast<Window *>(glfwGetWindowUserPointer(window)), x, y);
}

void CallbackFileDrop(GLFWwindow *window, int count, const char **files)
{
	Module_Window::HandleFileDrop(static_cast<Window *>(glfwGetWindowUserPointer(window)), count, files);
}


//------------------------------ WIN32 Event Process Middle Man --------------------------------------

#ifdef _WIN32
WNDPROC g_glfwWindowProc = nullptr;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window *window = (Window *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (uMsg) {
	case WM_DPICHANGED: {
		WORD x = LOWORD(wParam);
		WORD y = HIWORD(wParam);
		Module_Window::HandleDPIChange(window, x, y);
	}	break;
	}

	return CallWindowProc(g_glfwWindowProc, hWnd, uMsg, wParam, lParam);
}
#endif


//--------------------------------------------------------------------

IMPLEMENT_MODULE(Module_Window, Phase_Startup, true)
MODULE_DEPENDENCY(Module_Window, Module_Game, true)
MODULE_DEPENDENCY(Module_Window, Module_UI, true)

bool Module_Window::ModuleInit(Game *game)
{
#ifdef _WIN32
	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
#endif

#if OPENBBG_WITH_VULKAN
	windows.push_back(Window::CreateVulkanWindow(1280, 720, "OpenBBG"));
#endif

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

	// TEMP
	for (auto window : s_moduleInstance->windows)
		if (window->renderer.ptr != nullptr && glfwGetWindowAttrib(window->glfwWindow, GLFW_ICONIFIED) == GLFW_FALSE) {
			switch (window->rendererType) {
#if OPENBBG_WITH_VULKAN
			case RendererType::RendererType_Vulkan:
				window->renderer.vulkan->Render();
				break;
#endif
			case RendererType::RendererType_None:
			default:
				break;
			}
		}
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

	// Window Callbacks
	glfwSetWindowPosCallback(window->glfwWindow, CallbackPositionChange);
	glfwSetWindowSizeCallback(window->glfwWindow, CallbackSizeChange);
	glfwSetWindowCloseCallback(window->glfwWindow, CallbackClose);
	glfwSetWindowRefreshCallback(window->glfwWindow, CallbackRefresh);
	glfwSetWindowFocusCallback(window->glfwWindow, CallbackFocusChange);
	glfwSetWindowIconifyCallback(window->glfwWindow, CallbackIconifyChange);
	glfwSetFramebufferSizeCallback(window->glfwWindow, CallbackFramebufferSizeChange);
	
	// Input Callbacks
	glfwSetCharCallback(window->glfwWindow, CallbackCharacter);
	glfwSetCursorEnterCallback(window->glfwWindow, CallbackCursorEnter);
	glfwSetCursorPosCallback(window->glfwWindow, CallbackCursorPositionChange);
	glfwSetKeyCallback(window->glfwWindow, CallbackKey);
	glfwSetMouseButtonCallback(window->glfwWindow, CallbackMouseButton);
	glfwSetScrollCallback(window->glfwWindow, CallbackScroll);
	glfwSetDropCallback(window->glfwWindow, CallbackFileDrop);
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
	Game::Get()->jobsFrameStart.Queue([window, x, y]() {
		switch (window->rendererType) {
#if OPENBBG_WITH_VULKAN
			case RendererType::RendererType_Vulkan:
				window->renderer.vulkan->ResizeFramebuffer(x, y);
				break;
#endif
			case RendererType::RendererType_None:
			default:
				break;
		}
		window->framebufferSize.x = x;
		window->framebufferSize.y = y;
	});
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

void Module_Window::HandleCursorEnter(Window *window, bool isCursorIn)
{
}

void Module_Window::HandleCursorPositionChange(Window *window, double x, double y)
{
}

void Module_Window::HandleKey(Window *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window->glfwWindow, GLFW_TRUE);
}

void Module_Window::HandleMouseButton(Window *window, int button, int action, int mods)
{
}

void Module_Window::HandleScroll(Window *window, double x, double y)
{
}

void Module_Window::HandleFileDrop(Window *window, int count, const char **files)
{
}


}
