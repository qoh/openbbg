#ifndef _OPENBBG__RENDERER__VULKAN_GLOBALINSTANCE_H_
#define _OPENBBG__RENDERER__VULKAN_GLOBALINSTANCE_H_

namespace openbbg {
namespace vk {

typedef struct GlobalInstance GlobalInstance;

struct GlobalInstance
{
	bool isInitialized;

	VkInstance instance;

	VkSurfaceKHR surface;
    VkFormat surfaceFormat;
	
    vector<VkPhysicalDevice> physicalDevices;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    VkDevice device;

    int width;
	int height;

	// Queue Families
    uint32_t numQueueFamilies;
    vector<VkQueueFamilyProperties> queueFamilyProperties;
    uint32_t qfiGraphics;
    uint32_t qfiPresent;

	// Queues
	uint32_t numGraphicsQueues;
    vector<VkQueue> graphicsQueues;
    VkQueue presentQueue;

	CommandPool primaryCommandPool;

	uint32_t numSwapchainImages;
    VkSwapchainKHR swapchain;
    uint32_t currentSwapchainBufferIdx;

	// Render Nodes
	RenderNode *renderNode;

    VkPipelineCache pipelineCache;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VkSwapchainCreateInfoKHR swapchainCreateInfo;

	// Global Descriptor Sets
    VkDescriptorPool descGlobalParamPool;
    vector<VkDescriptorSet> descGlobalParamSets;

	GlobalInstance();

	~GlobalInstance();


	//- Instance -

	bool CreateInstance(const char *appSimpleName);
	

	//- Device -

	bool EnumerateDevices(uint32_t numPhysicalDevices = 1);

	bool InitSwapChainExtension(GLFWwindow *glfwWindow);

	bool CreateDevice();

	bool CreateQueues();
	
	bool CreateSwapChain(VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
	
	bool CreatePipelineCache();


	//------------------------------------------------------------------------------
	
	bool DestroySwapChain();


	//------------------------------------------------------------------------------

	bool Init(const char *appSimpleName, GLFWwindow *window);

	void Cleanup();
};

}
}
#endif

// Definitions
#include <OpenBBG/Renderer/Vulkan_GlobalInstance_def.h>
