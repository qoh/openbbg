#ifndef _OPENBBG__RENDERER__VULKAN_STAGEDBUFFER_H_
#define _OPENBBG__RENDERER__VULKAN_STAGEDBUFFER_H_

namespace openbbg {
namespace vk {

typedef struct StagedBuffer StagedBuffer;

struct StagedBuffer
{
	StagedBuffer(VkBufferUsageFlags flags);

	~StagedBuffer();

	void Init(uint64_t numBytes);

	void Cleanup();

	void CopyBegin(VkCommandBuffer *cmdBuffer);

	void CopyEnd(VkCommandBuffer *cmdBuffer);

	void CopyToDevice(VkBufferCopy copyRegion);

	void CopyToDevice(vector<VkBufferCopy> copyRegions);

	void MapMemory(void **ptr);

	void UnmapMemory();

	VkBufferUsageFlags flags;

	VkBuffer deviceBufferObject;
	VkDeviceMemory deviceBufferMemory;
	VkDescriptorBufferInfo deviceBufferInfo;
	VkBuffer stageBufferObject;
	VkDeviceMemory stageBufferMemory;
	VkDescriptorBufferInfo stageBufferInfo;
};

}
}
#endif

// Definitions
#include <OpenBBG/Renderer/Vulkan_StagedBuffer_def.h>
