#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Window.h>
#include <OpenBBG/Modules/Module_Window.h>

namespace openbbg {

#if OPENBBG_WITH_VULKAN
Window *Window::CreateVulkanWindow(int width, int height, const char *title)
{
	Window *window = new Window();
	window->rendererType = RendererType_Vulkan;
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	window->glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
	glfwSetWindowUserPointer(window->glfwWindow, window);

	glfwGetFramebufferSize(window->glfwWindow, &window->framebufferSize.x, &window->framebufferSize.y);
	window->renderer.vulkan = new Renderer_Vulkan(window);

	Module_Window::SetHandlers(window);

	return window;
}
#endif

Window::Window()
	: glfwWindow(nullptr)
	, rendererType(RendererType_None)
{
	renderer.ptr = nullptr;
}

Window::~Window()
{
	switch (rendererType) {
#if OPENBBG_WITH_VULKAN
	case RendererType_Vulkan:
		delete renderer.vulkan;
#endif
		break;
	}
	glfwDestroyWindow(glfwWindow);
}

}