#ifndef _OPENBBG__RENDERER__VULKAN_BUFFER_DEF_H_
#define _OPENBBG__RENDERER__VULKAN_BUFFER_DEF_H_

namespace openbbg {
namespace vk {

inline
Buffer::Buffer(VkBufferUsageFlags flags)
	: flags(flags)
{
}

inline
Buffer::~Buffer()
{
}

inline
void
Buffer::Init(uint64_t numBytes)
{
	auto r = Renderer_Vulkan::Get();
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
Buffer::Cleanup()
{
	auto r = Renderer_Vulkan::Get();
	vkDestroyBuffer(r->global.device, bufferObject, nullptr);
	vkFreeMemory(r->global.device, bufferMemory, nullptr);
}

inline
void
Buffer::MapMemory(void **ptr)
{
	auto r = Renderer_Vulkan::Get();
	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(r->global.device, bufferObject, &memReqs);
	VkResult res = vkMapMemory(r->global.device, bufferMemory, 0, memReqs.size, 0, ptr);
	assert(res == VK_SUCCESS);
}

inline
void
Buffer::UnmapMemory()
{
	auto r = Renderer_Vulkan::Get();
	vkUnmapMemory(r->global.device, bufferMemory);
}

}
}
#endif
