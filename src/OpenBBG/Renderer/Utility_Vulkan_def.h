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

}
#endif
