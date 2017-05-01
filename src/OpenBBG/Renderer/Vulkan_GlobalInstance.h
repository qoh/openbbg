#ifndef _OPENBBG__RENDERER__VULKAN_GLOBALINSTANCE_H_
#define _OPENBBG__RENDERER__VULKAN_GLOBALINSTANCE_H_

#define OPENBBG_VULKAN_ENABLE_VALIDATION_LAYERS 0

#if OPENBBG_VULKAN_ENABLE_VALIDATION_LAYERS
static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char *layerPrefix,
	const char *msg,
	void *userData);
#endif

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
	CommandPool transientCommandPool;

	uint32_t numSwapchainImages;
    VkSwapchainKHR swapchain;
    uint32_t currentSwapchainBufferIdx;

	// Render Nodes
	RenderNode *renderNode;

    VkPipelineCache pipelineCache;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VkSwapchainCreateInfoKHR swapchainCreateInfo;

	// Global Buffers
	VkBuffer globalParamBufferObject;
	VkDeviceMemory globalParamBufferMemory;
	VkDescriptorBufferInfo globalParamBufferInfo;

	// Global Descriptor Sets
	VkDescriptorSetLayout descGlobalParamLayout;
    VkDescriptorPool descGlobalParamPool;
    vector<VkDescriptorSet> descGlobalParamSets;

	VkSemaphore imageAcquiredSemaphore;
	VkFence drawFence;

    VkViewport viewport;
    VkRect2D scissor;

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
	

	//- Global Buffers -
	
    glm::mat4 Projection;
    
	glm::mat4 View;
    
	glm::mat4 Model;
    
	glm::mat4 Clip;

	glm::mat4 MVP;

	bool CreateGlobalBuffers();

	void UpdateMVP();

	void UploadGlobalParamsBuffer();


	//- Global Descriptor Pools & Sets -

	bool CreateGlobalDescriptorPoolsAndSets();


	//- Utility Functions -

	bool CreateBufferObject(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer &bufferObject, VkDeviceMemory &bufferMemory, VkMemoryRequirements *memReqReturn = nullptr);

	void SetViewport(float x, float y, float w, float h);

	void SetScissor(int32_t x, int32_t y, int32_t w, int32_t h);


#if OPENBBG_VULKAN_ENABLE_VALIDATION_LAYERS
	//- Validation Callback -

	VkDebugReportCallbackEXT callback;
#endif


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
