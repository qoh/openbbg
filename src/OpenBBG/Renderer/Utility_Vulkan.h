#ifndef _OPENBBG__RENDERER__UTILITY_VULKAN_H_
#define _OPENBBG__RENDERER__UTILITY_VULKAN_H_

// OpenBBG
#include <OpenBBG/Config.h>

// GLFW
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#if OPENBBG_WITH_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT
#define FENCE_TIMEOUT 100000000

#define XYZ1(_x_, _y_, _z_) (_x_), (_y_), (_z_), 1.f
#define UV(_u_, _v_) (_u_), (_v_)

namespace openbbg {

bool GetMemoryTypeFromProperties(VkPhysicalDeviceMemoryProperties &deviceMemoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);

}

// OpenBBG
#include <OpenBBG/Renderer/Vulkan_RenderNode.h>
#include <OpenBBG/Renderer/Vulkan_Pipeline.h>
#include <OpenBBG/Renderer/Vulkan_CommandPool.h>
#include <OpenBBG/Renderer/Vulkan_GlobalInstance.h>

#endif

// Definitions
#include <OpenBBG/Renderer/Utility_Vulkan_def.h>
