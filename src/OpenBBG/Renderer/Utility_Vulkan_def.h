#ifndef _OPENBBG__RENDERER__UTILITY_VULKAN_DEF_H_
#define _OPENBBG__RENDERER__UTILITY_VULKAN_DEF_H_

namespace openbbg {
	
inline
bool
GetMemoryTypeFromProperties(VkPhysicalDeviceMemoryProperties &deviceMemoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex)
{
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

inline
bool
CreateBufferObject(vk::GlobalInstance &global, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer &bufferObject, VkDeviceMemory &bufferMemory, VkMemoryRequirements *memReqReturn)
{
	// TODO: Populate error messages

	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usageFlags;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (VK_SUCCESS != vkCreateBuffer(global.device, &bufferCreateInfo, nullptr, &bufferObject))
		return false;

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(global.device, bufferObject, &memReq);
	if (memReqReturn != nullptr)
		memcpy(memReqReturn, &memReq, sizeof(memReq));

	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memReq.size;
	if (false == GetMemoryTypeFromProperties(global.deviceMemoryProperties, memReq.memoryTypeBits, propertyFlags, &allocateInfo.memoryTypeIndex))
		return false;
	if (VK_SUCCESS != vkAllocateMemory(global.device, &allocateInfo, nullptr, &bufferMemory))
		return false;
	if (VK_SUCCESS != vkBindBufferMemory(global.device, bufferObject, bufferMemory, 0))
		return false;

	return true;
}

}
#endif
