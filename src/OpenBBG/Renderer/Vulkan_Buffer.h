#ifndef _OPENBBG__RENDERER__VULKAN_BUFFER_H_
#define _OPENBBG__RENDERER__VULKAN_BUFFER_H_

namespace openbbg {
namespace vk {

typedef struct Buffer Buffer;

struct Buffer
{
	Buffer(VkBufferUsageFlags flags);

	~Buffer();

	void Init(Renderer_Vulkan *r, uint64_t numBytes);

	void Cleanup(Renderer_Vulkan *r);

	void MapMemory(Renderer_Vulkan *r, void **ptr);

	void UnmapMemory(Renderer_Vulkan *r);

	VkBufferUsageFlags flags;

	VkBuffer bufferObject;
	VkDeviceMemory bufferMemory;
	VkDescriptorBufferInfo bufferInfo;
};

}
}
#endif

// Definitions
#include <OpenBBG/Renderer/Vulkan_Buffer_def.h>
