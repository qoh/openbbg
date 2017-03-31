#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Window.h>
#include <OpenBBG/Modules/Module_Window.h>

namespace openbbg {

Window *Window::CreateVulkanWindow(int width, int height, const char *title)
{
	Window *window = new Window();
	window->rendererType = RendererType_Vulkan;
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window->glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
	glfwSetWindowUserPointer(window->glfwWindow, window);

	glfwGetFramebufferSize(window->glfwWindow, &window->framebufferSize.x, &window->framebufferSize.y);
	window->renderer.vulkan = new Renderer_Vulkan(window);

	Module_Window::SetHandlers(window);

	return window;
}

Window::Window()
	: glfwWindow(nullptr)
	, rendererType(RendererType_None)
{
	renderer.ptr = nullptr;
}

Window::~Window()
{
	switch (rendererType) {
	case RendererType_Vulkan:
		delete renderer.vulkan;
		break;
	}
	glfwDestroyWindow(glfwWindow);
}

}