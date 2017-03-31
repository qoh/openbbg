#pragma once

namespace openbbg {
namespace vk {
	
typedef struct GlobalInstance GlobalInstance;
typedef struct RenderNode RenderNode;
typedef struct ImageViewPair ImageViewPair;

struct ImageViewPair
{
	VkImage image;
	VkImageView view;
	
    VkFormat format;
    VkDeviceMemory mem;
};

struct RenderNode
{
	// Render Node Dependencies

	deque<RenderNode *> dependencies;

	bool hasPresent;

	bool presentToWindow;

	// -- Vulkan Objects --

	uint32_t numSamples;
	VkSampleCountFlagBits numSamplesBit;

	struct AttachmentDescRefPair
	{
		VkAttachmentDescription desc;
		VkAttachmentReference outputRef;
		VkAttachmentReference inputRef;
	};
	struct SubpassDescDepPair
	{
		VkSubpassDescription desc;
		VkSubpassDependency dep;
	};

	size_t numAttachments;
	std::vector<VkAttachmentDescription> attachmentDescs;
	std::vector<VkAttachmentReference> attachmentOutputRefs;
	std::vector<VkAttachmentReference> attachmentInputRefs;

	size_t numSubpasses;
	std::vector<VkSubpassDescription> subpassDescs;
	std::vector<VkSubpassDependency> subpassDeps;

    int width;
	int height;

	// Attachments
	// Subpasses
	// Dependency Graph for Subpasses
	// Renderpass

	std::vector<VkFramebuffer> framebuffers;
	
	bool hasRenderPass;
    VkRenderPass renderPass;
	uint32_t numFramebuffers;

	std::vector<ImageViewPair> primaryImagePairs;
	std::vector<ImageViewPair> imagePairs;

	RenderNode(uint32_t numSamples, uint32_t numFramebuffers, bool hasPresent)
		: hasPresent { hasPresent }
		, numAttachments { 0 }
		, numSamples { numSamples }
		, numSubpasses { 0 }
		, hasRenderPass { false }
		, numFramebuffers { numFramebuffers }
		, primaryImagePairs { numFramebuffers }
	{
		// TODO: Confirm if this is a correct assumption
		numSamplesBit = (VkSampleCountFlagBits)numSamples;
	}

	inline void SetAttachments(std::vector<AttachmentDescRefPair> attachmentInfo)
	{
		numAttachments = attachmentInfo.size();
		attachmentDescs.resize(numAttachments);
		attachmentOutputRefs.resize(numAttachments);
		attachmentInputRefs.resize(numAttachments);
		for (size_t a = 0; a < numAttachments; ++a) {
			attachmentDescs[a] = attachmentInfo[a].desc;
			attachmentOutputRefs[a] = attachmentInfo[a].outputRef;
			attachmentInputRefs[a] = attachmentInfo[a].inputRef;
		}

		imagePairs.resize(numAttachments);
	}

	inline void SetSubpasses(std::vector<SubpassDescDepPair> subpassInfo)
	{
		numSubpasses = subpassInfo.size();
		subpassDescs.resize(numSubpasses);
		subpassDeps.resize(numSubpasses);
		for (size_t a = 0; a < numSubpasses; ++a) {
			subpassDescs[a] = subpassInfo[a].desc;
			subpassDeps[a] = subpassInfo[a].dep;
		}
	}

	~RenderNode()
	{
	}

	inline bool CreateRenderPass(VkDevice &device)
	{
		if (hasRenderPass)
			return true;

		VkRenderPassCreateInfo rpCreateInfo = {};
		rpCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		rpCreateInfo.pNext = nullptr;
		rpCreateInfo.attachmentCount = (uint32_t)numAttachments;
		rpCreateInfo.pAttachments = attachmentDescs.data();
		rpCreateInfo.subpassCount = (uint32_t)numSubpasses;
		rpCreateInfo.pSubpasses = subpassDescs.data();
		if (subpassDeps.empty() == false && subpassDeps[0].dstSubpass != subpassDeps[0].srcSubpass) {
			rpCreateInfo.dependencyCount = (uint32_t)subpassDeps.size();
			rpCreateInfo.pDependencies = subpassDeps.data();
		} else {
			rpCreateInfo.dependencyCount = 0;
			rpCreateInfo.pDependencies = nullptr;
		}

		VkResult res = vkCreateRenderPass(device, &rpCreateInfo, nullptr, &renderPass);
		assert(res == VK_SUCCESS);

		hasRenderPass = true;
		return true;
	}

	inline void SetResolution(int w, int h)
	{
		width = w;
		height = h;
		// TODO: Resize framebuffers and images if allocated
	}

	inline bool CreateImagesAndViews(VkDevice &device, VkPhysicalDevice &physicalDevice, VkPhysicalDeviceMemoryProperties &deviceMemoryProperties)
	{
		uint32_t start = hasPresent ? 1 : 0;
		for (uint32_t a = start; a < numAttachments; ++a) {
			auto &attachmentDesc = attachmentDescs[a];
			auto &ivPair = imagePairs[a];

			VkResult res;
			bool pass;
			VkImageCreateInfo imageCreateInfo = {};

			const VkFormat attachmentFormat = attachmentDesc.format;
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, attachmentFormat, &props);
			if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
			else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			else
				imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.pNext = nullptr;
			imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			imageCreateInfo.format = attachmentFormat;
			imageCreateInfo.extent.width = width;
			imageCreateInfo.extent.height = height;
			imageCreateInfo.extent.depth = 1;
			imageCreateInfo.mipLevels = 1;
			imageCreateInfo.arrayLayers = 1;
			imageCreateInfo.samples = numSamplesBit;
			imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageCreateInfo.queueFamilyIndexCount = 0;
			imageCreateInfo.pQueueFamilyIndices = nullptr;
			imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageCreateInfo.flags = 0;

			VkMemoryAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.allocationSize = 0;
			allocateInfo.memoryTypeIndex = 0;

			VkImageViewCreateInfo ivCreateInfo = {};
			ivCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			ivCreateInfo.pNext = nullptr;
			ivCreateInfo.image = VK_NULL_HANDLE;
			ivCreateInfo.format = attachmentFormat;
			ivCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
			ivCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
			ivCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
			ivCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
			ivCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			ivCreateInfo.subresourceRange.baseMipLevel = 0;
			ivCreateInfo.subresourceRange.levelCount = 1;
			ivCreateInfo.subresourceRange.baseArrayLayer = 0;
			ivCreateInfo.subresourceRange.layerCount = 1;
			ivCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			ivCreateInfo.flags = 0;

			if (attachmentFormat == VK_FORMAT_D16_UNORM_S8_UINT
					|| attachmentFormat == VK_FORMAT_D24_UNORM_S8_UINT
					|| attachmentFormat == VK_FORMAT_D32_SFLOAT_S8_UINT) {
				ivCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}

			VkMemoryRequirements memoryReqs;

			res = vkCreateImage(device, &imageCreateInfo, nullptr, &ivPair.image);
			assert(res == VK_SUCCESS);

			vkGetImageMemoryRequirements(device, ivPair.image, &memoryReqs);

			allocateInfo.allocationSize = memoryReqs.size;
			pass = memory_type_from_properties(deviceMemoryProperties, memoryReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &allocateInfo.memoryTypeIndex);
			assert(pass);

			res = vkAllocateMemory(device, &allocateInfo, nullptr, &ivPair.mem);
			assert(res == VK_SUCCESS);

			res = vkBindImageMemory(device, ivPair.image, ivPair.mem, 0);
			assert(res == VK_SUCCESS);

			ivCreateInfo.image = ivPair.image;
			res = vkCreateImageView(device, &ivCreateInfo, nullptr, &ivPair.view);
			assert(res == VK_SUCCESS);
		}

		return true;
	}

	inline bool CreateSwapchainViews(VkDevice &device, VkSwapchainKHR &swapchain)
	{
		if (hasPresent == false)
			return false;

		VkResult res;

		std::vector<VkImage> swapchainImages(numFramebuffers);
		res = vkGetSwapchainImagesKHR(device, swapchain, &numFramebuffers, swapchainImages.data());
		assert(res == VK_SUCCESS);

		for (uint32_t a = 0; a < numFramebuffers; ++a) {
			ImageViewPair &ivPair = primaryImagePairs[a];

			VkImageViewCreateInfo ivCreateInfo = {};
			ivCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			ivCreateInfo.pNext = nullptr;
			ivCreateInfo.format = attachmentDescs[0].format;
			ivCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
			ivCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
			ivCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
			ivCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
			ivCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ivCreateInfo.subresourceRange.baseMipLevel = 0;
			ivCreateInfo.subresourceRange.levelCount = 1;
			ivCreateInfo.subresourceRange.baseArrayLayer = 0;
			ivCreateInfo.subresourceRange.layerCount = 1;
			ivCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			ivCreateInfo.flags = 0;

			ivPair.image = swapchainImages[a];
			ivCreateInfo.image = ivPair.image;

			res = vkCreateImageView(device, &ivCreateInfo, nullptr, &ivPair.view);
			assert(res == VK_SUCCESS);
		}

		return true;
	}

	inline bool CreateFramebuffers(VkDevice &device)
	{
		framebuffers.resize(numFramebuffers);

		VkResult res;
		std::vector<VkImageView> imageViews(numAttachments);
		uint32_t start = hasPresent ? 1 : 0;
		for (uint32_t a = start; a < numAttachments; ++a)
			imageViews[a] = imagePairs[a].view;

		VkFramebufferCreateInfo fbCreateInfo = {};
		fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbCreateInfo.pNext = nullptr;
		fbCreateInfo.renderPass = renderPass;
		fbCreateInfo.attachmentCount = (uint32_t)numAttachments;
		fbCreateInfo.pAttachments = imageViews.data();
		fbCreateInfo.width = width;
		fbCreateInfo.height = height;
		fbCreateInfo.layers = 1;

		for (uint32_t a = 0; a < numFramebuffers; a++) {
			if (hasPresent)
				imageViews[0] = primaryImagePairs[a].view;
			res = vkCreateFramebuffer(device, &fbCreateInfo, nullptr, &framebuffers[a]);
			assert(res == VK_SUCCESS);
		}

		return true;
	}

	inline void Cleanup(VkDevice &device)
	{
		if (hasRenderPass == false)
			return;

		{
			uint32_t start = hasPresent ? 1 : 0;
			for (uint32_t a = start; a < numAttachments; ++a) {
				auto &ivPair = imagePairs[a];
				vkDestroyImageView(device, ivPair.view, nullptr);
				vkDestroyImage(device, ivPair.image, nullptr);
				vkFreeMemory(device, ivPair.mem, nullptr);
			}
		}

		for (uint32_t a = 0; a < numFramebuffers; ++a)
			vkDestroyImageView(device, primaryImagePairs[a].view, nullptr);

		for (uint32_t a = 0; a < numFramebuffers; ++a)
			vkDestroyFramebuffer(device, framebuffers[a], nullptr);

		vkDestroyRenderPass(device, renderPass, nullptr);

		hasRenderPass = false;
	}
};

}
}