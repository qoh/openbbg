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
#define NUM_DESCRIPTOR_SETS 1
#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS
#define FENCE_TIMEOUT 100000000

namespace openbbg {

bool GetMemoryTypeFromProperties(VkPhysicalDeviceMemoryProperties &deviceMemoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);

}

// OpenBBG
#include <OpenBBG/Renderer/Vulkan_RenderNode.h>
#include <OpenBBG/Renderer/Vulkan_Pipeline.h>
#include <OpenBBG/Renderer/Vulkan_CommandPool.h>
#include <OpenBBG/Renderer/Vulkan_GlobalInstance.h>

namespace openbbg {

//---------------------------------------------

struct texture_object {
    VkSampler sampler;

    VkImage image;
    VkImageLayout imageLayout;

    VkDeviceMemory mem;
    VkImageView view;
    int32_t tex_width, tex_height;
};

struct sample_info {
#ifdef _WIN32
#else
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_window_t window;
    xcb_intern_atom_reply_t *atom_wm_delete_window;
#endif

    VkSemaphore imageAcquiredSemaphore;

    vector<struct texture_object> textures;

    struct {
        VkDescriptorImageInfo image_info;
    } texture_data;
    VkDeviceMemory stagingMemory;
    VkImage stagingImage;

    struct {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo buffer_info;
    } vertex_buffer;

    VkDescriptorPool desc_pool;
    vector<VkDescriptorSet> desc_set;

    PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback;
    PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback;
    PFN_vkDebugReportMessageEXT dbgBreakCallback;
    vector<VkDebugReportCallbackEXT> debug_report_callbacks;

    VkViewport viewport;
    VkRect2D scissor;
};

//------------------------

inline void init_vertex_buffer(vk::GlobalInstance &global, struct sample_info &info, const void *vertexData, uint32_t dataSize, uint32_t dataStride,
                        bool use_texture)
{
	VkMemoryRequirements memReqs;
	assert(global.CreateBufferObject(dataSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, info.vertex_buffer.buf, info.vertex_buffer.mem, &memReqs));
	info.vertex_buffer.buffer_info.range = memReqs.size;
	info.vertex_buffer.buffer_info.offset = 0;

	uint8_t *pData;
	assert(VK_SUCCESS == vkMapMemory(global.device, info.vertex_buffer.mem, 0, memReqs.size, 0, (void **)&pData));

	memcpy(pData, vertexData, dataSize);

	vkUnmapMemory(global.device, info.vertex_buffer.mem);
}

inline void init_descriptor_pool(vk::GlobalInstance &global, struct sample_info &info, bool use_texture)
{
    /* DEPENDS on init_uniform_buffer() and
     * init_descriptor_and_pipeline_layouts() */

    VkResult res;
    VkDescriptorPoolSize type_count[2];
    type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    type_count[0].descriptorCount = 1;
    if (use_texture) {
        type_count[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        type_count[1].descriptorCount = 1;
    }

    VkDescriptorPoolCreateInfo descriptor_pool = {};
    descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool.pNext = NULL;
    descriptor_pool.maxSets = 1;
    descriptor_pool.poolSizeCount = use_texture ? 2 : 1;
    descriptor_pool.pPoolSizes = type_count;

    res = vkCreateDescriptorPool(global.device, &descriptor_pool, NULL, &info.desc_pool);
    assert(res == VK_SUCCESS);
}

inline void init_viewports(vk::GlobalInstance &global, struct sample_info &info)
{
    info.viewport.height = (float)global.height;
    info.viewport.width = (float)global.width;
    info.viewport.minDepth = (float)0.0f;
    info.viewport.maxDepth = (float)1.0f;
    info.viewport.x = 0;
    info.viewport.y = 0;
    vkCmdSetViewport(global.primaryCommandPool.currentBuffer, 0, NUM_VIEWPORTS, &info.viewport);
}

inline void init_scissors(vk::GlobalInstance &global, struct sample_info &info)
{
    info.scissor.extent.width = global.width;
    info.scissor.extent.height = global.height;
    info.scissor.offset.x = 0;
    info.scissor.offset.y = 0;
    vkCmdSetScissor(global.primaryCommandPool.currentBuffer, 0, NUM_SCISSORS, &info.scissor);
}

}
#endif

// Definitions
#include <OpenBBG/Renderer/Utility_Vulkan_def.h>
