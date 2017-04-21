#ifndef _OPENBBG__UI__UI_CLASS_COLORCTRL_H_
#define _OPENBBG__UI__UI_CLASS_COLORCTRL_H_

// OpenBBG
#include <OpenBBG/UI/UI_Class.h>

namespace openbbg {

typedef struct UI_Class_ColorCtrl UI_Class_ColorCtrl;

struct UI_Class_ColorCtrl
	: UI_Class
	, Singleton<UI_Class_ColorCtrl>
{
	UI_Class_ColorCtrl();

	~UI_Class_ColorCtrl();

	virtual UI_Control *Construct();

#if OPENBBG_WITH_VULKAN
	virtual void Init(Renderer_Vulkan *r);

	virtual void Cleanup(Renderer_Vulkan *r);

	virtual void Cleanup(Renderer_Vulkan *r, UI_Context *ctx);

	virtual void Cleanup(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl);

	virtual void Prepare(Renderer_Vulkan *r, UI_Context *ctx);

	virtual void Prepare(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl);

	virtual void RenderOpaque(Renderer_Vulkan *r, UI_Context *ctx);

	virtual void RenderTransparent(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl);

	virtual void RenderOverlay(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl);

	VkPipeline pipeline;

	vk::GraphicsPipeline *graphicsPipeline;

	// Local Data
	typedef struct LocalDataEntry
	{
		glm::vec2 position;
		glm::vec2 extent;
		glm::vec4 color;
		glm::vec4 scissor;
		glm::vec2 hz;
	} LocalDataEntry;

	vector<LocalDataEntry> entries;

	typedef struct LocalData {
		VkDrawIndirectCommand indirectCommand;

		VkBuffer indirectBufferObject;
		VkDeviceMemory indirectBufferMemory;
		VkDescriptorBufferInfo indirectBufferInfo;
		VkBuffer indirectStagingBufferObject;
		VkDeviceMemory indirectStagingBufferMemory;
		VkDescriptorBufferInfo indirectStagingBufferInfo;

		VkBuffer localBufferObject;
		VkDeviceMemory localBufferMemory;
		VkDescriptorBufferInfo localBufferInfo;
		VkBuffer localStagingBufferObject;
		VkDeviceMemory localStagingBufferMemory;
		VkDescriptorBufferInfo localStagingBufferInfo;

		VkDescriptorSetLayout descLocalLayout;
		VkDescriptorPool descLocalPool;
		vector<VkDescriptorSet> descLocalSets;
	} LocalData;

	map<UI_Context *, LocalData> localDataMap;

	void CreateLocalData(Renderer_Vulkan *r, UI_Context *ctx);
	void UploadLocalData(Renderer_Vulkan *r, LocalData &data);

	// Instance Buffer
	VkBuffer instanceBufferObject;
	VkDeviceMemory instanceBufferMemory;
	VkDescriptorBufferInfo instanceBufferInfo;

	// Vertex Buffer
	VkBuffer vertexBufferObject;
	VkDeviceMemory vertexBufferMemory;
	VkDescriptorBufferInfo vertexBufferInfo;
#endif
};

}
#endif
