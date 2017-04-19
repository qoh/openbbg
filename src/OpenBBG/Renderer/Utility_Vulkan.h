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

bool CreateBufferObject(vk::GlobalInstance &global, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer &bufferObject, VkDeviceMemory &bufferMemory, VkMemoryRequirements *memReqReturn = nullptr);

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

    bool prepared;
    bool use_staging_buffer;
    bool save_images;

    VkSemaphore imageAcquiredSemaphore;

    vector<struct texture_object> textures;

    struct {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo buffer_info;
    } uniform_data;

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

    glm::mat4 Projection;
    glm::mat4 View;
    glm::mat4 Model;
    glm::mat4 Clip;
    glm::mat4 MVP;

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

inline void init_uniform_buffer(vk::GlobalInstance &global, struct sample_info &info)
{
	float fov = glm::radians(45.0f);
	if (global.width > global.height) {
		fov *= static_cast<float>(global.height) / static_cast<float>(global.width);
	}
	info.Projection = glm::perspective(fov, static_cast<float>(global.width) / static_cast<float>(global.height), 0.1f, 100.0f);
	info.View = glm::lookAt(glm::vec3(-5, 3, -10),  // Camera is at (-5,3,-10), in World Space
			glm::vec3(0, 0, 0),     // and looks at the origin
			glm::vec3(0, -1, 0)     // Head is up (set to 0,-1,0 to look upside-down)
	);
	info.Model = glm::mat4(1.0f);
	// Vulkan clip space has inverted Y and half Z.
	info.Clip = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);

	info.MVP = info.Clip * info.Projection * info.View * info.Model;

	/* VULKAN_KEY_START */
	VkMemoryRequirements memReqs;
	assert(CreateBufferObject(global, sizeof(info.MVP), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, info.uniform_data.buf, info.uniform_data.mem, &memReqs));

	uint8_t *pData;
	assert(VK_SUCCESS == vkMapMemory(global.device, info.uniform_data.mem, 0, sizeof(info.MVP), 0, (void **)&pData));

	memcpy(pData, &info.MVP, sizeof(info.MVP));

	vkUnmapMemory(global.device, info.uniform_data.mem);

	info.uniform_data.buffer_info.buffer = info.uniform_data.buf;
	info.uniform_data.buffer_info.offset = 0;
	info.uniform_data.buffer_info.range = sizeof(info.MVP);
}

inline void init_vertex_buffer(vk::GlobalInstance &global, struct sample_info &info, const void *vertexData, uint32_t dataSize, uint32_t dataStride,
                        bool use_texture)
{
	VkMemoryRequirements memReqs;
	assert(CreateBufferObject(global, dataSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, info.vertex_buffer.buf, info.vertex_buffer.mem, &memReqs));
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


inline void init_descriptor_set(vk::GlobalInstance &global, struct sample_info &info, vk::GraphicsPipeline *gp, bool use_texture)
{
    /* DEPENDS on init_descriptor_pool() */

    VkResult res;

    VkDescriptorSetAllocateInfo alloc_info[1];
    alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info[0].pNext = NULL;
    alloc_info[0].descriptorPool = info.desc_pool;
    alloc_info[0].descriptorSetCount = NUM_DESCRIPTOR_SETS;
    alloc_info[0].pSetLayouts = gp->desc_layout.data();

    info.desc_set.resize(NUM_DESCRIPTOR_SETS);
    res = vkAllocateDescriptorSets(global.device, alloc_info, info.desc_set.data());
    assert(res == VK_SUCCESS);

    VkWriteDescriptorSet writes[2];

    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = NULL;
    writes[0].dstSet = info.desc_set[0];
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &info.uniform_data.buffer_info;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    vkUpdateDescriptorSets(global.device, use_texture ? 2 : 1, writes, 0, NULL);
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



inline void set_image_layout(vk::GlobalInstance &global, struct sample_info &info, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout,
                      VkImageLayout new_image_layout, VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages) {
    /* DEPENDS on global.primaryCommandPool.currentBuffer and info.queue initialized */

    assert(global.primaryCommandPool.currentBuffer != VK_NULL_HANDLE);
    assert(global.graphicsQueues[0] != VK_NULL_HANDLE);

    VkImageMemoryBarrier image_memory_barrier = {};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.pNext = NULL;
    image_memory_barrier.srcAccessMask = 0;
    image_memory_barrier.dstAccessMask = 0;
    image_memory_barrier.oldLayout = old_image_layout;
    image_memory_barrier.newLayout = new_image_layout;
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange.aspectMask = aspectMask;
    image_memory_barrier.subresourceRange.baseMipLevel = 0;
    image_memory_barrier.subresourceRange.levelCount = 1;
    image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier.subresourceRange.layerCount = 1;

    switch (old_image_layout) {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;

        default:
            break;
    }

    switch (new_image_layout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        default:
            break;
    }

    vkCmdPipelineBarrier(global.primaryCommandPool.currentBuffer, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);
}

}
#endif

// Definitions
#include <OpenBBG/Renderer/Utility_Vulkan_def.h>
