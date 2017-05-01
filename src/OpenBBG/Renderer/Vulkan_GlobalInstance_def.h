#ifndef _OPENBBG__RENDERER__VULKAN_GLOBALINSTANCE_DEF_H_
#define _OPENBBG__RENDERER__VULKAN_GLOBALINSTANCE_DEF_H_

#if OPENBBG_VULKAN_ENABLE_VALIDATION_LAYERS
static VKAPI_ATTR
VkBool32 VKAPI_CALL
VulkanDebugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char *layerPrefix,
	const char *msg,
	void *userData)
{
	using namespace openbbg;
	if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
		LOG_INFO("[vulkan] {}", msg);
	} else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
		LOG_WARN("[vulkan] {}", msg);
	} else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
		LOG_WARN("[vulkan] {}", msg);
	} else if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
		LOG_ERROR("[vulkan] {}", msg);
	} else if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
		LOG_DEBUG("[vulkan] {}", msg);
	}
	return VK_FALSE;
}
#endif

namespace openbbg {
namespace vk {

inline
GlobalInstance::GlobalInstance()
	: isInitialized { false }
	, primaryCommandPool { device, qfiGraphics, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT }
	, transientCommandPool { device, qfiGraphics, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT }
	, renderNode { nullptr }
	, swapchainCreateInfo {}
{
	viewport.maxDepth = 1.f;
	viewport.minDepth = 0.f;
}

inline
GlobalInstance::~GlobalInstance()
{
	Cleanup();
}


//- Instance -

inline
bool
GlobalInstance::CreateInstance(const char *appSimpleName)
{
	// Layers
	vector<const char *> layerNames;
#if OPENBBG_VULKAN_ENABLE_VALIDATION_LAYERS
	const vector<const char *> validationLayers = {
	//	"VK_LAYER_LUNARG_api_dump",
		"VK_LAYER_LUNARG_core_validation",
		"VK_LAYER_LUNARG_monitor",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_LUNARG_screenshot",
		"VK_LAYER_LUNARG_swapchain",
		"VK_LAYER_GOOGLE_threading",
		"VK_LAYER_GOOGLE_unique_objects",
	//	"VK_LAYER_LUNARG_vktrace",
	//	"VK_LAYER_NV_optimus",
	//	"VK_LAYER_NV_nsight",
	//	"VK_LAYER_RENDERDOC_Capture",
	//	"VK_LAYER_VALVE_steam_overlay",
		"VK_LAYER_LUNARG_standard_validation"
	};

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char *layerName : validationLayers)
		for (const auto &layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerNames.push_back(layerName);
				break;
			}
		}
#endif


	// Extensions
	vector<const char *> extensionNames;
	extensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
	extensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
	extensionNames.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
#if OPENBBG_VULKAN_ENABLE_VALIDATION_LAYERS
	extensionNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
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

	VkResult res = vkCreateInstance(&instCreateInfo, nullptr, &instance);
	return res == VK_SUCCESS;
}
	

//- Device -

inline
bool
GlobalInstance::EnumerateDevices(uint32_t numPhysicalDevices)
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

inline
bool
GlobalInstance::InitSwapChainExtension(GLFWwindow *glfwWindow)
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
	res = vkCreateXcbSurfaceKHR(instance, &createInfo, nullptr, &surface);
#endif
	assert(res == VK_SUCCESS);

	// Query WSI support
	vector<VkBool32> supported(numQueueFamilies);
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
	vector<VkSurfaceFormatKHR> surfaceFormats(numFormats);
	res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[0], surface, &numFormats, surfaceFormats.data());
	assert(res == VK_SUCCESS && numFormats >= 1);
	surfaceFormat = (numFormats == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) ? VK_FORMAT_B8G8R8A8_UNORM : surfaceFormats[0].format;

	return true;
}

inline
bool
GlobalInstance::CreateDevice()
{
	vector<const char *> extensionNames;
	extensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	uint32_t numQueueCreates = qfiPresent == qfiGraphics ? 1 : 2;
	vector<VkDeviceQueueCreateInfo> queueCreateInfos(numQueueCreates);

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

	VkPhysicalDeviceFeatures supportedFeatures = {};
	vkGetPhysicalDeviceFeatures(physicalDevices[0], &supportedFeatures);

	VkPhysicalDeviceFeatures enabledFeatures = supportedFeatures;
	enabledFeatures.shaderClipDistance = supportedFeatures.shaderClipDistance & VK_TRUE;

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.queueCreateInfoCount = numQueueCreates;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.enabledExtensionCount = (uint32_t)extensionNames.size();
	deviceCreateInfo.ppEnabledExtensionNames = extensionNames.empty() ? nullptr : extensionNames.data();
	deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

	return vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device) == VK_SUCCESS;
}

inline
bool
GlobalInstance::CreateQueues()
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

	
inline
bool
GlobalInstance::CreateSwapChain(VkImageUsageFlags usageFlags)
{
	VkResult res;

	res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevices[0], surface, &surfaceCapabilities);
	assert(res == VK_SUCCESS);

	uint32_t numPresentModes;
	res = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevices[0], surface, &numPresentModes, nullptr);
	assert(res == VK_SUCCESS);

	vector<VkPresentModeKHR> presentModes(numPresentModes);
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
	VkPresentModeKHR swapchainPresentMode = find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) == presentModes.end() ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
#endif

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
	
inline
bool
GlobalInstance::CreatePipelineCache()
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


inline
bool
GlobalInstance::DestroySwapChain()
{
	vkDestroySwapchainKHR(device, swapchain, nullptr);
	return true;
}


//------------------------------------------------------------------------------

inline
bool
GlobalInstance::Init(const char *appSimpleName, GLFWwindow *window)
{
	if (isInitialized)
		return true;

	assert(CreateInstance(appSimpleName));

#if OPENBBG_VULKAN_ENABLE_VALIDATION_LAYERS
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = 0
	//	| VK_DEBUG_REPORT_INFORMATION_BIT_EXT
		| VK_DEBUG_REPORT_WARNING_BIT_EXT
		| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
		| VK_DEBUG_REPORT_ERROR_BIT_EXT
	//	| VK_DEBUG_REPORT_DEBUG_BIT_EXT
		;
	createInfo.pfnCallback = VulkanDebugCallback;
	auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (vkCreateDebugReportCallbackEXT != nullptr)
		vkCreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback);
#endif

	assert(EnumerateDevices());
	assert(InitSwapChainExtension(window));
	assert(CreateDevice());
	assert(CreateQueues());

	primaryCommandPool.Init();
	transientCommandPool.Init();

	primaryCommandPool.BeginCurrentBuffer();

	assert(CreateSwapChain());
	assert(CreatePipelineCache());



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
				(const VkAttachmentReference *)&renderNode->attachmentOutputRefs[0],
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

	assert(CreateGlobalBuffers());
	assert(CreateGlobalDescriptorPoolsAndSets());

	VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
	imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	imageAcquiredSemaphoreCreateInfo.pNext = nullptr;
	imageAcquiredSemaphoreCreateInfo.flags = 0;
	assert(VK_SUCCESS == vkCreateSemaphore(device, &imageAcquiredSemaphoreCreateInfo, nullptr, &imageAcquiredSemaphore));
	
	VkFenceCreateInfo fenceInfo;
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = nullptr;
	fenceInfo.flags = 0;
	vkCreateFence(device, &fenceInfo, nullptr, &drawFence);

	isInitialized = true;
	return true;
}

inline
void
GlobalInstance::Cleanup()
{
	if (isInitialized == false)
		return;
		
	if (renderNode != nullptr) {
		renderNode->Cleanup(device);
		delete renderNode;
		renderNode = nullptr;
	}

	//---------------------------

	vkDestroySemaphore(device, imageAcquiredSemaphore, nullptr);

	vkDestroyFence(device, drawFence, nullptr);

	//----------------------------
	
//	vkFreeDescriptorSets(device, descGlobalParamPool, (uint32_t)descGlobalParamSets.size(), descGlobalParamSets.data());

	vkDestroyDescriptorPool(device, descGlobalParamPool, nullptr);

	vkDestroyDescriptorSetLayout(device, descGlobalParamLayout, nullptr);

	//----------------------------

	// Global Buffers
	
	vkDestroyBuffer(device, globalParamBufferObject, nullptr);

	vkFreeMemory(device, globalParamBufferMemory, nullptr);


	//---------------------------

		
	vkDestroyPipelineCache(device, pipelineCache, nullptr);

	DestroySwapChain();

	transientCommandPool.Cleanup();
	primaryCommandPool.Cleanup();

	vkDestroyDevice(device, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);

#if OPENBBG_VULKAN_ENABLE_VALIDATION_LAYERS
	auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (vkDestroyDebugReportCallbackEXT != nullptr)
		vkDestroyDebugReportCallbackEXT(instance, callback, nullptr);
#endif

	vkDestroyInstance(instance, nullptr);

	isInitialized = false;
}

inline
void
GlobalInstance::SetViewport(float x, float y, float w, float h)
{
    viewport.x = x;
    viewport.y = y;
    viewport.width = w;
    viewport.height = h;
    vkCmdSetViewport(primaryCommandPool.currentBuffer, 0, 1, &viewport);
}

inline
void
GlobalInstance::SetScissor(int32_t x, int32_t y, int32_t w, int32_t h)
{
    scissor.offset.x = x;
    scissor.offset.y = y;
    scissor.extent.width = w;
    scissor.extent.height = h;
    vkCmdSetScissor(primaryCommandPool.currentBuffer, 0, 1, &scissor);
}

inline
bool
GlobalInstance::CreateBufferObject(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer &bufferObject, VkDeviceMemory &bufferMemory, VkMemoryRequirements *memReqReturn)
{
	// TODO: Populate error messages

	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usageFlags;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (VK_SUCCESS != vkCreateBuffer(device, &bufferCreateInfo, nullptr, &bufferObject))
		return false;

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, bufferObject, &memReq);
	if (memReqReturn != nullptr)
		memcpy(memReqReturn, &memReq, sizeof(memReq));

	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memReq.size;
	if (false == GetMemoryTypeFromProperties(deviceMemoryProperties, memReq.memoryTypeBits, propertyFlags, &allocateInfo.memoryTypeIndex))
		return false;
	if (VK_SUCCESS != vkAllocateMemory(device, &allocateInfo, nullptr, &bufferMemory))
		return false;
	if (VK_SUCCESS != vkBindBufferMemory(device, bufferObject, bufferMemory, 0))
		return false;

	return true;
}

inline
bool
GlobalInstance::CreateGlobalBuffers()
{
	assert(CreateBufferObject(
		sizeof(glm::mat4),
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		globalParamBufferObject,
		globalParamBufferMemory,
		nullptr));
	
	globalParamBufferInfo = {
		globalParamBufferObject,
		0,
		sizeof(glm::mat4)
	};

	UpdateMVP();
	UploadGlobalParamsBuffer();

	return true;
}

inline
void
GlobalInstance::UpdateMVP()
{
#if 1
	Projection = glm::ortho(0.f, static_cast<float>(width), static_cast<float>(height), 0.f);
	View = glm::mat4(1.0f);
#else
	float fov = glm::radians(45.0f);
	if (width > height)
		fov *= static_cast<float>(height) / static_cast<float>(width);
	Projection = glm::perspective(fov, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
	View = glm::lookAt(glm::vec3(-5, 3, -10),  // Camera is at (-5,3,-10), in World Space
							glm::vec3(0, 0, 0),     // and looks at the origin
							glm::vec3(0, -1, 0)     // Head is up (set to 0,-1,0 to look upside-down)
							);
#endif
	Model = glm::mat4(1.0f);

	// Vulkan clip space has inverted Y and half Z.
	Clip = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);

	MVP = Clip * Projection * View * Model;
}

inline
void
GlobalInstance::UploadGlobalParamsBuffer()
{
	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(device, globalParamBufferObject, &memReqs);

	uint8_t *pData;
	VkResult res = vkMapMemory(device, globalParamBufferMemory, 0, memReqs.size, 0, (void **)&pData);
	assert(res == VK_SUCCESS);

	memcpy(pData, &MVP, sizeof(MVP));

	vkUnmapMemory(device, globalParamBufferMemory);
}

inline
bool
GlobalInstance::CreateGlobalDescriptorPoolsAndSets()
{
	VkResult res;

	// Global Params
	{
		VkDescriptorSetLayoutBinding descSetLayoutBinding {
			0,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			1,
			VK_SHADER_STAGE_VERTEX_BIT,
			nullptr
		};

		VkDescriptorSetLayoutCreateInfo descSetLayoutCreateInfo;
		descSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descSetLayoutCreateInfo.pNext = nullptr;
		descSetLayoutCreateInfo.flags = 0;
		descSetLayoutCreateInfo.bindingCount = 1;
		descSetLayoutCreateInfo.pBindings = &descSetLayoutBinding;

		res = vkCreateDescriptorSetLayout(device, &descSetLayoutCreateInfo, nullptr, &descGlobalParamLayout);
		assert(res == VK_SUCCESS);

		VkDescriptorPoolSize descPoolSize;
		descPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descPoolSize.descriptorCount = 1;

		VkDescriptorPoolCreateInfo descPoolCreateInfo = {};
		descPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descPoolCreateInfo.pNext = nullptr;
		descPoolCreateInfo.maxSets = 1;
		descPoolCreateInfo.poolSizeCount = 1;
		descPoolCreateInfo.pPoolSizes = &descPoolSize;

		res = vkCreateDescriptorPool(device, &descPoolCreateInfo, nullptr, &descGlobalParamPool);
		assert(res == VK_SUCCESS);

		VkDescriptorSetAllocateInfo descSetAllocateInfo;
		descSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descSetAllocateInfo.pNext = nullptr;
		descSetAllocateInfo.descriptorPool = descGlobalParamPool;
		descSetAllocateInfo.descriptorSetCount = 1;
		descSetAllocateInfo.pSetLayouts = &descGlobalParamLayout;

		descGlobalParamSets.resize(1);
		res = vkAllocateDescriptorSets(device, &descSetAllocateInfo, descGlobalParamSets.data());
		assert(res == VK_SUCCESS);

		VkWriteDescriptorSet writeDescSet = {};
		writeDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescSet.pNext = nullptr;
		writeDescSet.dstSet = descGlobalParamSets[0];
		writeDescSet.descriptorCount = 1;
		writeDescSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescSet.pBufferInfo = &globalParamBufferInfo;
		writeDescSet.dstArrayElement = 0;
		writeDescSet.dstBinding = 0;

		vkUpdateDescriptorSets(device, 1, &writeDescSet, 0, nullptr);
	}

	return true;
}

}
}
#endif
