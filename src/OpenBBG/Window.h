#pragma once

// OpenBBG
#include <OpenBBG/Renderer/Renderer_Vulkan.h>

// GLFW
#if OPENBBG_WITH_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#if linux
#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h>

namespace openbbg {

typedef struct Window Window;

typedef enum RendererType {
	RendererType_None,
	RendererType_Vulkan
} RendererType;

typedef union Renderer {
	Renderer_Vulkan *vulkan;
	void *ptr;
} Renderer;

struct Window
{
	GLFWwindow *glfwWindow;

	RendererType rendererType;

	Renderer renderer;

	glm::ivec2 framebufferSize;

	static Window *CreateVulkanWindow(int width, int height, const char *title);

	Window();

	~Window();
};

}