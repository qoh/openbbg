#ifndef _OPENBBG__RENDERER__VULKAN_STAGEDBUFFER_DEF_H_
#define _OPENBBG__RENDERER__VULKAN_STAGEDBUFFER_DEF_H_

namespace openbbg {
namespace vk {

inline
StagedBuffer::StagedBuffer(VkBufferUsageFlags flags)
{
}

inline
StagedBuffer::~StagedBuffer()
{
}

inline
void
StagedBuffer::Init(Renderer_Vulkan *r, uint64_t numBytes)
{
	assert(r->global.CreateBufferObject(
		numBytes,
		flags | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		deviceBufferObject,
		deviceBufferMemory,
		nullptr));
	assert(r->global.CreateBufferObject(
		numBytes,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stageBufferObject,
		stageBufferMemory,
		nullptr));
	
	deviceBufferInfo = {
		deviceBufferObject,
		0,
		numBytes
	};
	stageBufferInfo = {
		stageBufferObject,
		0,
		numBytes
	};
}

inline
void
StagedBuffer::Cleanup(Renderer_Vulkan *r)
{
	vkDestroyBuffer(r->global.device, stageBufferObject, nullptr);
	vkFreeMemory(r->global.device, stageBufferMemory, nullptr);
	vkDestroyBuffer(r->global.device, deviceBufferObject, nullptr);
	vkFreeMemory(r->global.device, deviceBufferMemory, nullptr);
}

inline
void
StagedBuffer::CopyToDevice(Renderer_Vulkan *r, VkBufferCopy copyRegion)
{
	VkCommandBuffer commandBuffer;
	CopyBegin(r, &commandBuffer);

	vkCmdCopyBuffer(commandBuffer, stageBufferObject, deviceBufferObject, 1, &copyRegion);

	CopyEnd(r, &commandBuffer);
}

inline
void
StagedBuffer::CopyToDevice(Renderer_Vulkan *r, vector<VkBufferCopy> copyRegions)
{
	VkCommandBuffer commandBuffer;
	CopyBegin(r, &commandBuffer);

	vkCmdCopyBuffer(commandBuffer, stageBufferObject, deviceBufferObject, (uint32_t)copyRegions.size(), copyRegions.data());

	CopyEnd(r, &commandBuffer);
}

inline
void
StagedBuffer::CopyBegin(Renderer_Vulkan *r, VkCommandBuffer *cmdBuffer)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = r->global.transientCommandPool.pool;
	allocInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(r->global.device, &allocInfo, cmdBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(*cmdBuffer, &beginInfo);
}

inline
void
StagedBuffer::CopyEnd(Renderer_Vulkan *r, VkCommandBuffer *cmdBuffer)
{
	vkEndCommandBuffer(*cmdBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = cmdBuffer;

	vkQueueSubmit(r->global.graphicsQueues[0], 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(r->global.graphicsQueues[0]);

	vkFreeCommandBuffers(r->global.device, r->global.transientCommandPool.pool, 1, cmdBuffer);
}

inline
void
StagedBuffer::MapMemory(Renderer_Vulkan *r, void **ptr)
{
	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(r->global.device, stageBufferObject, &memReqs);
	VkResult res = vkMapMemory(r->global.device, stageBufferMemory, 0, memReqs.size, 0, ptr);
	assert(res == VK_SUCCESS);
}

inline
void
StagedBuffer::UnmapMemory(Renderer_Vulkan *r)
{
	vkUnmapMemory(r->global.device, stageBufferMemory);
}

}
}
#endif
