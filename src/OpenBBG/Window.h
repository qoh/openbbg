#pragma once

// OpenBBG
#include <OpenBBG/Config.h>
#if OPENBBG_WITH_VULKAN
#include <OpenBBG/Renderer/Renderer_Vulkan.h>
#endif

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
	  RendererType_None
#if OPENBBG_WITH_VULKAN
	, RendererType_Vulkan
#endif
} RendererType;

typedef union Renderer {
#if OPENBBG_WITH_VULKAN
	Renderer_Vulkan *vulkan;
#endif
	void *ptr;
} Renderer;

struct Window
{
	GLFWwindow *glfwWindow;

	RendererType rendererType;

	Renderer renderer;

	glm::ivec2 framebufferSize;

#if OPENBBG_WITH_VULKAN
	static Window *CreateVulkanWindow(int width, int height, const char *title);
#endif

	Window();

	~Window();
};

}