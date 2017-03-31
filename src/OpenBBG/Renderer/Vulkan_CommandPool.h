#pragma once

namespace openbbg {
namespace vk {

typedef struct CommandPool CommandPool;

struct CommandPool
{
	bool isInitialized;

    VkDevice &device;
    uint32_t &qfiGraphics;

    VkCommandPool pool;

	uint32_t currentBufferIdx;
	uint32_t numBuffers;
	std::vector<VkCommandBuffer> buffers;
	VkCommandBuffer currentBuffer;



	CommandPool(VkDevice &device, uint32_t &qfiGraphics)
		: isInitialized { false }
		, device(device)
		, qfiGraphics { qfiGraphics }
		, currentBufferIdx { 0 }
		, numBuffers { 0 }
	{
	}

	~CommandPool()
	{
		Cleanup();
	}
	
	inline bool CreatePool()
	{
		VkCommandPoolCreateInfo poolCreateInfo = {};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolCreateInfo.pNext = nullptr;
		poolCreateInfo.queueFamilyIndex = qfiGraphics;
		poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		return vkCreateCommandPool(device, &poolCreateInfo, nullptr, &pool) == VK_SUCCESS;
	}
	
	inline bool CreateBuffers(uint32_t count)
	{
		assert(numBuffers == 0);
		numBuffers = count;
		currentBufferIdx = count - 1;
		buffers.resize(count);

		VkCommandBufferAllocateInfo bufferAllocateInfo = {};
		bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		bufferAllocateInfo.pNext = nullptr;
		bufferAllocateInfo.commandPool = pool;
		bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		bufferAllocateInfo.commandBufferCount = count;

		return vkAllocateCommandBuffers(device, &bufferAllocateInfo, buffers.data()) == VK_SUCCESS;
	}

	//----------------------------

	inline bool Init()
	{
		if (isInitialized)
			return true;

		assert(CreatePool());
		assert(CreateBuffers(2));
		UpdateCurrentBuffer();

		isInitialized = true;
		return true;
	}

	inline void Cleanup()
	{
		if (isInitialized == false)
			return;
		
		vkFreeCommandBuffers(device, pool, numBuffers, buffers.data());

		vkDestroyCommandPool(device, pool, nullptr);

		numBuffers = 0;
		buffers.clear();
		isInitialized = false;
	}

	inline void UpdateCurrentBuffer()
	{
		currentBufferIdx = (currentBufferIdx + 1) % numBuffers;
		currentBuffer = buffers[currentBufferIdx];
	}

	inline void BeginCurrentBuffer()
	{
		VkResult res;

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pNext = nullptr;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		res = vkBeginCommandBuffer(currentBuffer, &beginInfo);
		assert(res == VK_SUCCESS);
	}

	inline void EndCurrentBuffer()
	{
		VkResult res;

		res = vkEndCommandBuffer(currentBuffer);
		assert(res == VK_SUCCESS);
	}
};

}
}