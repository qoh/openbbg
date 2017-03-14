#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Renderer/Renderer_Vulkan.h>
#include <OpenBBG/Window.h>
#include <OpenBBG/Log.h>

// GLFW
#if TEST_VULKAN
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

Renderer_Vulkan::Renderer_Vulkan(Window *window)
	: window(window)
{
}

Renderer_Vulkan::~Renderer_Vulkan()
{
	LOG_DEBUG("VULKAN DED");
}

}