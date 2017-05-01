#ifndef _OPENBBG__RENDERER__VULKAN_PIPELINE_H_
#define _OPENBBG__RENDERER__VULKAN_PIPELINE_H_

// OpenBBG
#include <OpenBBG/Renderer/Vulkan_ShaderUtility.h>

namespace openbbg {
namespace vk {

typedef struct GraphicsPipeline GraphicsPipeline;
typedef struct ComputePipeline ComputePipeline;

struct RenderNodeSubpassPair
{
	RenderNode *renderNode;
	uint32_t subpass;
};

bool operator <(const RenderNodeSubpassPair &a, const RenderNodeSubpassPair &b);

struct GraphicsPipeline
{
	static deque<GraphicsPipeline *> s_pipelines;

	bool isInitialized;
	
    VkPipelineLayout pipeline_layout;
    vector<VkDescriptorSetLayout> desc_layout;
	map<RenderNodeSubpassPair, VkPipeline> nodePipelineSubpassMap;
    VkVertexInputBindingDescription vi_binding;
    VkVertexInputAttributeDescription vi_attribs[2];
	
	vector<VkPipelineShaderStageCreateInfo> shaderStages;
	vector<const char *> glslSources;
	vector<vector<VkDescriptorSetLayoutBinding>> descriptorSetLayouts;
	vector<VkVertexInputBindingDescription> viBindingDescriptions;
	vector<VkVertexInputAttributeDescription> viAttributeDescriptions;
	VkPipelineVertexInputStateCreateInfo viStateCreateInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
	VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo;
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
	vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
	vector<VkDynamicState> dynamicStates;

	GraphicsPipeline(
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
			vector<VkDynamicState> dynamicStates);

	~GraphicsPipeline();

	bool CreateDescriptorAndPipelineLayouts(VkDevice &device);

	bool CreateShaderModules(VkDevice &device);

	VkPipeline CreatePipeline(VkDevice &device, VkPipelineCache &pipelineCache, RenderNode *renderNode, uint32_t subpass);

	bool Init();
	
	void Cleanup();
};


struct ComputePipeline
{
	bool isInitialized { false };

	bool Init();

	void Cleanup();
};

}
}
#endif

// Definitions
#include <OpenBBG/Renderer/Vulkan_Pipeline_def.h>
