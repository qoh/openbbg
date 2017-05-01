#ifndef _OPENBBG__RENDERER__VULKAN_STAGEDBUFFER_H_
#define _OPENBBG__RENDERER__VULKAN_STAGEDBUFFER_H_

namespace openbbg {
namespace vk {

typedef struct StagedBuffer StagedBuffer;

struct StagedBuffer
{
	StagedBuffer(VkBufferUsageFlags flags);

	~StagedBuffer();

	void Init(Renderer_Vulkan *r, uint64_t numBytes);

	void Cleanup(Renderer_Vulkan *r);

	void CopyBegin(Renderer_Vulkan *r, VkCommandBuffer *cmdBuffer);

	void CopyEnd(Renderer_Vulkan *r, VkCommandBuffer *cmdBuffer);

	void CopyToDevice(Renderer_Vulkan *r, VkBufferCopy copyRegion);

	void CopyToDevice(Renderer_Vulkan *r, vector<VkBufferCopy> copyRegions);

	void MapMemory(Renderer_Vulkan *r, void **ptr);

	void UnmapMemory(Renderer_Vulkan *r);

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
