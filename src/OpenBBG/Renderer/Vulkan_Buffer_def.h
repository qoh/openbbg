#ifndef _OPENBBG__RENDERER__VULKAN_BUFFER_DEF_H_
#define _OPENBBG__RENDERER__VULKAN_BUFFER_DEF_H_

namespace openbbg {
namespace vk {

inline
Buffer::Buffer(VkBufferUsageFlags flags)
{
}

inline
Buffer::~Buffer()
{
}

inline
void
Buffer::Init(Renderer_Vulkan *r, uint64_t numBytes)
{
	assert(r->global.CreateBufferObject(
		numBytes,
		flags,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		bufferObject,
		bufferMemory,
		nullptr));
	
	bufferInfo = {
		bufferObject,
		0,
		numBytes
	};
}

inline
void
Buffer::Cleanup(Renderer_Vulkan *r)
{
	vkDestroyBuffer(r->global.device, bufferObject, nullptr);
	vkFreeMemory(r->global.device, bufferMemory, nullptr);
}

inline
void
Buffer::MapMemory(Renderer_Vulkan *r, void **ptr)
{
	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(r->global.device, bufferObject, &memReqs);
	VkResult res = vkMapMemory(r->global.device, bufferMemory, 0, memReqs.size, 0, ptr);
	assert(res == VK_SUCCESS);
}

inline
void
Buffer::UnmapMemory(Renderer_Vulkan *r)
{
	vkUnmapMemory(r->global.device, bufferMemory);
}

}
}
#endif
