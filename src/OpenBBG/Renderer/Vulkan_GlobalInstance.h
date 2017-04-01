#pragma once

namespace openbbg {
namespace vk {

typedef struct GlobalInstance GlobalInstance;

struct GlobalInstance
{
	bool isInitialized;

	VkPipeline testPipeline;

	VkInstance instance;

	VkSurfaceKHR surface;
    VkFormat surfaceFormat;
	
    std::vector<VkPhysicalDevice> physicalDevices;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    VkDevice device;

    int width;
	int height;

	// Queue Families
    uint32_t numQueueFamilies;
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    uint32_t qfiGraphics;
    uint32_t qfiPresent;

	// Queues
	uint32_t numGraphicsQueues;
    std::vector<VkQueue> graphicsQueues;
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

	GlobalInstance()
		: isInitialized { false }
		, primaryCommandPool { device, qfiGraphics }
		, renderNode { nullptr }
		, swapchainCreateInfo {}
	{
	}

	~GlobalInstance()
	{
		Cleanup();
	}


	//- Instance -

	inline bool CreateInstance(const char *appSimpleName)
	{
		// Layers
		std::vector<const char *> layerNames;

		// Extensions
		std::vector<const char *> extensionNames;
		extensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
		extensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
		extensionNames.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = appSimpleName;
		appInfo.applicationVersion = 1;
		appInfo.pEngineName = appSimpleName;
		appInfo.engineVersion = 1;
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo instCreateInfo = {};
		instCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instCreateInfo.pNext = nullptr;
		instCreateInfo.flags = 0;
		instCreateInfo.pApplicationInfo = &appInfo;
		instCreateInfo.enabledLayerCount = (uint32_t)layerNames.size();
		instCreateInfo.ppEnabledLayerNames = layerNames.empty() ? nullptr : layerNames.data();
		instCreateInfo.enabledExtensionCount = (uint32_t)extensionNames.size();
		instCreateInfo.ppEnabledExtensionNames = extensionNames.empty() ? nullptr : extensionNames.data();

		return vkCreateInstance(&instCreateInfo, nullptr, &instance) == VK_SUCCESS;
	}
	

	//- Device -

	inline bool EnumerateDevices(uint32_t numPhysicalDevices = 1)
	{
		VkResult res;
		const uint32_t numRequestedPhysicalDevices = numPhysicalDevices;

		// Get handles to physical devices
		res = vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, nullptr);
		assert(numPhysicalDevices);
		physicalDevices.resize(numPhysicalDevices);

		res = vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, physicalDevices.data());
		assert(!res && numPhysicalDevices >= numRequestedPhysicalDevices);

		// Get device properties
		vkGetPhysicalDeviceMemoryProperties(physicalDevices[0], &deviceMemoryProperties);
		vkGetPhysicalDeviceProperties(physicalDevices[0], &deviceProperties);

		// Use first device, assuming it is the primary GPU

		// Get handles to queue families
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[0], &numQueueFamilies, nullptr);
		assert(numQueueFamilies >= 1);

		queueFamilyProperties.resize(numQueueFamilies);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[0], &numQueueFamilies, queueFamilyProperties.data());
		assert(numQueueFamilies >= 1);

		return true;
	}

	inline bool InitSwapChainExtension(GLFWwindow *glfwWindow)
	{
		VkResult res;

		// Create surface
#ifdef _WIN32
		res = glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface);
#else
		VkXcbSurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.connection = connection;
		createInfo.window = window;
		res = vkCreateXcbSurfaceKHR(instance, &createInfo, NULL, &surface);
#endif
		assert(res == VK_SUCCESS);

		// Query WSI support
		std::vector<VkBool32> supported(numQueueFamilies);
		for (uint32_t i = 0; i < numQueueFamilies; ++i)
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], i, surface, &supported[i]);

		// Query for queue family indices
		qfiGraphics = UINT32_MAX;
		qfiPresent = UINT32_MAX;
		for (uint32_t a = 0; a < numQueueFamilies; ++a) {
			if (queueFamilyProperties[a].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				if (qfiGraphics == UINT32_MAX)
					qfiGraphics = a;
				if (supported[a] == VK_TRUE) {
					qfiGraphics = a;
					qfiPresent = a;
					break;
				}
			}
		}
		if (qfiPresent == UINT32_MAX)
			for (uint32_t a = 0; a < numQueueFamilies; ++a)
				if (supported[a] == VK_TRUE) {
					qfiPresent = a;
					break;
				}

		assert(qfiGraphics != UINT32_MAX && qfiPresent != UINT32_MAX);

		// Determine surface format
		uint32_t numFormats;
		res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[0], surface, &numFormats, nullptr);
		assert(res == VK_SUCCESS);
		std::vector<VkSurfaceFormatKHR> surfaceFormats(numFormats);
		res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[0], surface, &numFormats, surfaceFormats.data());
		assert(res == VK_SUCCESS && numFormats >= 1);
		surfaceFormat = (numFormats == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) ? VK_FORMAT_B8G8R8A8_UNORM : surfaceFormats[0].format;

		return true;
	}

	inline bool CreateDevice()
	{
		std::vector<const char *> extensionNames;
		extensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		uint32_t numQueueCreates = qfiPresent == qfiGraphics ? 1 : 2;
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(numQueueCreates);

		// Graphics Queue
		{
			numGraphicsQueues = 1;
			float queuePriorities[1] = { 1.0 };
			queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfos[0].pNext = nullptr;
			queueCreateInfos[0].queueCount = 1;
			queueCreateInfos[0].pQueuePriorities = queuePriorities;
			queueCreateInfos[0].queueFamilyIndex = qfiGraphics;
		}

		// Present Queue
		if (qfiPresent != qfiGraphics) {
			float queuePriorities[1] = { 1.0 };
			queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfos[1].pNext = nullptr;
			queueCreateInfos[1].queueCount = 1;
			queueCreateInfos[1].pQueuePriorities = queuePriorities;
			queueCreateInfos[1].queueFamilyIndex = qfiPresent;
		}

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = nullptr;
		deviceCreateInfo.queueCreateInfoCount = numQueueCreates;
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.enabledExtensionCount = (uint32_t)extensionNames.size();
		deviceCreateInfo.ppEnabledExtensionNames = extensionNames.empty() ? nullptr : extensionNames.data();
		deviceCreateInfo.pEnabledFeatures = nullptr;

		return vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device) == VK_SUCCESS;
	}

	inline bool CreateQueues()
	{
		graphicsQueues.resize(numGraphicsQueues);
		for (uint32_t a = 0; a < numGraphicsQueues; ++a)
			vkGetDeviceQueue(device, qfiGraphics, a, &graphicsQueues[a]);
		if (qfiGraphics == qfiPresent)
			presentQueue = graphicsQueues[0];
		else
			vkGetDeviceQueue(device, qfiPresent, 0, &presentQueue);
		return true;
	}

	
	inline bool CreateSwapChain(VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
	{
		VkResult res;

		res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevices[0], surface, &surfaceCapabilities);
		assert(res == VK_SUCCESS);

		uint32_t numPresentModes;
		res = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevices[0], surface, &numPresentModes, nullptr);
		assert(res == VK_SUCCESS);

		std::vector<VkPresentModeKHR> presentModes(numPresentModes);
		res = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevices[0], surface, &numPresentModes, presentModes.data());
		assert(res == VK_SUCCESS);

		VkExtent2D swapchainExtent;
		if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
			swapchainExtent.width = std::min(std::max((uint32_t)width, surfaceCapabilities.minImageExtent.width), surfaceCapabilities.maxImageExtent.width);
			swapchainExtent.height = std::min(std::max((uint32_t)height, surfaceCapabilities.minImageExtent.height), surfaceCapabilities.maxImageExtent.height);
		} else
			swapchainExtent = surfaceCapabilities.currentExtent;
		
#if OPENBBG_VULKAN_VSYNC
		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
#else
		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
#endif
	//	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

		uint32_t desiredNumberOfSwapchainImages = surfaceCapabilities.minImageCount;

		VkSurfaceTransformFlagBitsKHR preTransform;
		if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		} else {
			preTransform = surfaceCapabilities.currentTransform;
		}
		
		uint32_t queueFamilyIndices[2] = { qfiGraphics, qfiPresent };
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.pNext = nullptr;
		swapchainCreateInfo.surface = surface;
		swapchainCreateInfo.minImageCount = desiredNumberOfSwapchainImages;
		swapchainCreateInfo.imageFormat = surfaceFormat;
		swapchainCreateInfo.imageExtent.width = swapchainExtent.width;
		swapchainCreateInfo.imageExtent.height = swapchainExtent.height;
		swapchainCreateInfo.preTransform = preTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.presentMode = swapchainPresentMode;
		swapchainCreateInfo.imageExtent.width = swapchainExtent.width;
		swapchainCreateInfo.imageExtent.height = swapchainExtent.height;
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
		swapchainCreateInfo.clipped = true;
		swapchainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		swapchainCreateInfo.imageUsage = usageFlags;
		if (qfiGraphics != qfiPresent) {
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCreateInfo.queueFamilyIndexCount = 0;
			swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		}

		swapchainCreateInfo.imageExtent.width = swapchainExtent.width;
		swapchainCreateInfo.imageExtent.height = swapchainExtent.height;
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
		res = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);
		assert(res == VK_SUCCESS);

		res = vkGetSwapchainImagesKHR(device, swapchain, &numSwapchainImages, nullptr);
		assert(res == VK_SUCCESS);

		currentSwapchainBufferIdx = 0;

		return true;
	}
	
	inline bool CreatePipelineCache()
	{
		VkResult res;
		
		VkPipelineCacheCreateInfo pcCreateInfo;
		pcCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		pcCreateInfo.pNext = nullptr;
		pcCreateInfo.initialDataSize = 0;
		pcCreateInfo.pInitialData = nullptr;
		pcCreateInfo.flags = 0;
		res = vkCreatePipelineCache(device, &pcCreateInfo, nullptr, &pipelineCache);
		assert(res == VK_SUCCESS);

		return true;
	}


	//------------------------------------------------------------------------------
	
	inline bool DestroySwapChain()
	{
		vkDestroySwapchainKHR(device, swapchain, nullptr);
	}


	//------------------------------------------------------------------------------

	inline bool Init(const char *appSimpleName, GLFWwindow *window)
	{
		if (isInitialized)
			return true;

		assert(CreateInstance(appSimpleName));
		assert(EnumerateDevices());
		assert(InitSwapChainExtension(window));
		assert(CreateDevice());
		assert(CreateQueues());

		primaryCommandPool.Init();
		primaryCommandPool.BeginCurrentBuffer();

		assert(CreateSwapChain());
		assert(CreatePipelineCache());



#if 1
		renderNode = new RenderNode(1, numSwapchainImages, true);
		renderNode->SetAttachments({
			// Back Buffer
			{
				{
					0,
					surfaceFormat,
					NUM_SAMPLES,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				}, {
					0,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
				}, {
					0,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				}
			},
			// Depth Buffer
			{
				{
					0,
					VK_FORMAT_D16_UNORM,
					NUM_SAMPLES,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				}, {
					1,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				}, {
					1,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
				}
			}
		});
		
		renderNode->SetSubpasses({
			// Present
			{
				{
					0,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					0,
					nullptr,
					1,
					(const VkAttachmentReference *)renderNode->attachmentDescs.data(),
					nullptr,
					(const VkAttachmentReference *)&renderNode->attachmentOutputRefs[1],
					0,
					nullptr
				}, {
					0,
					0,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT
				}
			}
		});

		renderNode->SetResolution(width, height);
		assert(renderNode->CreateRenderPass(device));
		assert(renderNode->CreateSwapchainViews(device, swapchain));
		assert(renderNode->CreateImagesAndViews(device, physicalDevices[0], deviceMemoryProperties));
		assert(renderNode->CreateFramebuffers(device));
#else
		renderNode = new RenderNode({
			// Back Buffer
			{
				{
					0,
					VK_FORMAT_R8G8B8A8_UNORM,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				}, {
					0,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
				}, {
					0,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				}
			},

			// Depth Buffer
			{
				{
					0,
					VK_FORMAT_D32_SFLOAT,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_UNDEFINED
				}, {
					1,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				}, {
					1,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
				}
			},

			// G-Buffer 1
			{
				{
					0,
					VK_FORMAT_R32G32B32A32_UINT,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_UNDEFINED
				}, {
					2,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
				}, {
					2,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				}
			},
		}, 1);


		// Subpass Descs
		// Subpass Deps
		// RenderPassCreateInfo
		// Create RenderPass
#endif










		isInitialized = true;
		return true;
	}

	inline void Cleanup()
	{
		if (isInitialized == false)
			return;
		
		if (renderNode != nullptr) {
			renderNode->Cleanup(device);
			delete renderNode;
			renderNode = nullptr;
		}

		
		vkDestroyPipelineCache(device, pipelineCache, nullptr);

		DestroySwapChain();
		
		primaryCommandPool.Cleanup();

		vkDestroyDevice(device, nullptr);
		
		vkDestroySurfaceKHR(instance, surface, nullptr);

		vkDestroyInstance(instance, nullptr);

		isInitialized = false;
	}
};

}
}