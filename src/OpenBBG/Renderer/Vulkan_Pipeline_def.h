#ifndef _OPENBBG__RENDERER__VULKAN_PIPELINE_DEF_H_
#define _OPENBBG__RENDERER__VULKAN_PIPELINE_DEF_H_

namespace openbbg {
namespace vk {

inline
bool
operator <(const RenderNodeSubpassPair &a, const RenderNodeSubpassPair &b)
{
    return std::tie(a.renderNode, a.subpass) < std::tie(b.renderNode, b.subpass);
}

inline
GraphicsPipeline::GraphicsPipeline(
		vector<VkPipelineShaderStageCreateInfo> shaderStages,
		vector<const char *> glslSources,
		vector<vector<VkDescriptorSetLayoutBinding>> descriptorSetLayouts,
		vector<VkVertexInputBindingDescription> viBindingDescriptions,
		vector<VkVertexInputAttributeDescription> viAttributeDescriptions,
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo,
		VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo,
		VkPipelineViewportStateCreateInfo viewportStateCreateInfo,
		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo,
		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo,
		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo,
		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo,
		vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments,
		vector<VkDynamicState> dynamicStates)
	: isInitialized { false }
	, shaderStages { shaderStages }
	, glslSources { glslSources }
	, descriptorSetLayouts { descriptorSetLayouts }
	, viBindingDescriptions { viBindingDescriptions }
	, viAttributeDescriptions { viAttributeDescriptions }
	, inputAssemblyStateCreateInfo { inputAssemblyStateCreateInfo }
	, tessellationStateCreateInfo { tessellationStateCreateInfo }
	, viewportStateCreateInfo { viewportStateCreateInfo }
	, rasterizationStateCreateInfo { rasterizationStateCreateInfo }
	, multisampleStateCreateInfo { multisampleStateCreateInfo }
	, depthStencilStateCreateInfo { depthStencilStateCreateInfo }
	, colorBlendStateCreateInfo { colorBlendStateCreateInfo }
	, colorBlendAttachments { colorBlendAttachments }
	, dynamicStates { dynamicStates }
	, viStateCreateInfo {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		0,
		nullptr,
		0,
		nullptr,
	}
	, dynamicStateCreateInfo {
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		nullptr,
		0,
		0,
		nullptr,
	}
{
	this->colorBlendStateCreateInfo.attachmentCount = (uint32_t)colorBlendAttachments.size();
	this->colorBlendStateCreateInfo.pAttachments = this->colorBlendAttachments.data();

	this->viStateCreateInfo.vertexBindingDescriptionCount = (uint32_t)viBindingDescriptions.size();
	this->viStateCreateInfo.pVertexBindingDescriptions = this->viBindingDescriptions.data();
	this->viStateCreateInfo.vertexAttributeDescriptionCount = (uint32_t)viAttributeDescriptions.size();
	this->viStateCreateInfo.pVertexAttributeDescriptions = this->viAttributeDescriptions.data();
		
	this->dynamicStateCreateInfo.dynamicStateCount = (uint32_t)dynamicStates.size();
	this->dynamicStateCreateInfo.pDynamicStates = this->dynamicStates.data();

	s_pipelines.push_back(this);
}

inline
GraphicsPipeline::~GraphicsPipeline()
{
	s_pipelines.erase(std::remove(s_pipelines.begin(), s_pipelines.end(), this), s_pipelines.end());
}

inline
bool
GraphicsPipeline::CreateDescriptorAndPipelineLayouts(VkDevice &device)
{
	uint32_t numDescriptorSetLayouts = (uint32_t)descriptorSetLayouts.size();
	vector<VkDescriptorSetLayoutCreateInfo> descriptorSetLayoutCreateInfos(numDescriptorSetLayouts);
	for (uint32_t a = 0; a < numDescriptorSetLayouts; ++a) {
		auto &descriptorSetLayoutCreateInfo = descriptorSetLayoutCreateInfos[a];
		auto &descriptorSetLayout = descriptorSetLayouts[a];
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = (uint32_t)descriptorSetLayout.size();
		descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayout.data();
	}

	VkResult res;

	desc_layout.resize(numDescriptorSetLayouts);
	for (uint32_t a = 0; a < numDescriptorSetLayouts; ++a) {
		res = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfos[a], nullptr, &desc_layout[a]);
		assert(res == VK_SUCCESS);
	}

	VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
	pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pPipelineLayoutCreateInfo.pNext = nullptr;
	pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pPipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
	pPipelineLayoutCreateInfo.setLayoutCount = numDescriptorSetLayouts;
	pPipelineLayoutCreateInfo.pSetLayouts = desc_layout.data();

	res = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &pipeline_layout);
	assert(res == VK_SUCCESS);

	return true;
}

inline
bool
GraphicsPipeline::CreateShaderModules(VkDevice &device)
{
	VkResult res;
	bool retVal;

	if (shaderStages.empty())
		return false;

	glslang::InitializeProcess();
		
	VkShaderModuleCreateInfo moduleCreateInfo;
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = nullptr;
	moduleCreateInfo.flags = 0;

	uint32_t numShaderStages = (uint32_t)shaderStages.size();
	for (uint32_t a = 0; a < numShaderStages; ++a) {
		auto &shaderStage = shaderStages[a];
		vector<unsigned int> spvCode;
		retVal = GLSLtoSPV(shaderStage.stage, glslSources[a], spvCode);
		assert(retVal);
		moduleCreateInfo.codeSize = spvCode.size() * sizeof(unsigned int);
		moduleCreateInfo.pCode = spvCode.data();

		res = vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderStage.module);
		assert(res == VK_SUCCESS);
	}

	glslang::FinalizeProcess();

	return true;
}

inline
VkPipeline
GraphicsPipeline::CreatePipeline(VkDevice &device, VkPipelineCache &pipelineCache, RenderNode *renderNode, uint32_t subpass)
{
	VkResult res;

	RenderNodeSubpassPair nsPair = { renderNode, subpass };
	auto search = nodePipelineSubpassMap.find(nsPair);
	if (search != nodePipelineSubpassMap.end())
		return search->second;

	VkGraphicsPipelineCreateInfo pipelineCreateInfo;
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.pNext = nullptr;
	pipelineCreateInfo.flags = 0;
	pipelineCreateInfo.stageCount = (uint32_t)shaderStages.size();
	pipelineCreateInfo.pStages = shaderStages.data();
	pipelineCreateInfo.pVertexInputState = viStateCreateInfo.sType == 0 ? nullptr : &viStateCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = inputAssemblyStateCreateInfo.sType == 0 ? nullptr : &inputAssemblyStateCreateInfo;
	pipelineCreateInfo.pTessellationState = tessellationStateCreateInfo.sType == 0 ? nullptr : &tessellationStateCreateInfo;
	pipelineCreateInfo.pViewportState = viewportStateCreateInfo.sType == 0 ? nullptr : &viewportStateCreateInfo;
	pipelineCreateInfo.pRasterizationState = rasterizationStateCreateInfo.sType == 0 ? nullptr : &rasterizationStateCreateInfo;
	pipelineCreateInfo.pMultisampleState = multisampleStateCreateInfo.sType == 0 ? nullptr : &multisampleStateCreateInfo;
	pipelineCreateInfo.pDepthStencilState = depthStencilStateCreateInfo.sType == 0 ? nullptr : &depthStencilStateCreateInfo;
	pipelineCreateInfo.pColorBlendState = colorBlendStateCreateInfo.sType == 0 ? nullptr : &colorBlendStateCreateInfo;
	pipelineCreateInfo.pDynamicState = dynamicStateCreateInfo.sType == 0 ? nullptr : &dynamicStateCreateInfo;
	pipelineCreateInfo.layout = pipeline_layout;
	pipelineCreateInfo.renderPass = renderNode->renderPass;
	pipelineCreateInfo.subpass = subpass;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = 0;

	VkPipeline pipeline;
	res = vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline);
	assert(res == VK_SUCCESS);

	nodePipelineSubpassMap[nsPair] = pipeline;

	return pipeline;
}

inline
bool
GraphicsPipeline::Init()
{
	if (isInitialized)
		return true;

	auto r = Renderer_Vulkan::Get();
	auto &device = r->global.device;
	assert(CreateDescriptorAndPipelineLayouts(device));
	assert(CreateShaderModules(device));

	isInitialized = true;
	return true;
}

inline
void
GraphicsPipeline::Cleanup()
{
	if (isInitialized == false)
		return;
		
	auto r = Renderer_Vulkan::Get();
	auto &device = r->global.device;
	for (auto &entry : nodePipelineSubpassMap)
		vkDestroyPipeline(device, entry.second, nullptr);
		
	for (auto &stage : shaderStages)
		vkDestroyShaderModule(device, stage.module, nullptr);
		
	for (auto &desc : desc_layout)
		vkDestroyDescriptorSetLayout(device, desc, nullptr);
	vkDestroyPipelineLayout(device, pipeline_layout, nullptr);

	isInitialized = false;
}

inline
bool
ComputePipeline::Init()
{
	if (isInitialized)
		return true;

	isInitialized = true;
	return true;
}
	
inline
void
ComputePipeline::Cleanup()
{
	if (isInitialized == false)
		return;
		
	isInitialized = false;
}

}
}
#endif
