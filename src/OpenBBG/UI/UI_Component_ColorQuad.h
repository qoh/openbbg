#ifndef _OPENBBG__UI__UI_COMPONENT_COLORQUAD_H_
#define _OPENBBG__UI__UI_COMPONENT_COLORQUAD_H_

// OpenBBG
#include <OpenBBG/UI/UI_Component.h>
#include <OpenBBG/Renderer/Vulkan_StagedBuffer.h>

namespace openbbg {

typedef struct UI_Component_ColorQuad UI_Component_ColorQuad;
typedef struct UI_ComponentInstance_ColorQuad UI_ComponentInstance_ColorQuad;

struct UI_ComponentInstance_ColorQuad
	: UI_ComponentInstance
{
	glm::vec4 color;
};


struct UI_Component_ColorQuad
	: UI_Component
	, Singleton<UI_Component_ColorQuad>
{
	UI_Component_ColorQuad();

	virtual ~UI_Component_ColorQuad();

	virtual UI_ComponentInstance *Construct();

	virtual void Deconstruct(UI_ComponentInstance *compInst);

#if OPENBBG_WITH_VULKAN
	virtual void Init(Renderer_Vulkan *r);

	virtual void Cleanup(Renderer_Vulkan *r);

	virtual void Cleanup(Renderer_Vulkan *r, UI_Context *ctx);

	virtual void Cleanup(Renderer_Vulkan *r, UI_Context *ctx, UI_ComponentInstance *compInst);

	virtual void Prepare(Renderer_Vulkan *r, UI_Context *ctx);

	virtual void Prepare(Renderer_Vulkan *r, UI_Context *ctx, UI_ComponentInstance *compInst);

	virtual void RenderOpaque(Renderer_Vulkan *r, UI_Context *ctx);

	virtual void RenderTransparent(Renderer_Vulkan *r, UI_Context *ctx, vector<UI_ComponentInstance *> &instances, uint32_t startInstance, uint32_t numInstances);

	virtual void RenderOverlay(Renderer_Vulkan *r, UI_Context *ctx, UI_ComponentInstance *compInst);

	virtual void PopulateTransparentInstances(Renderer_Vulkan *r, UI_Context *ctx, vector<UI_ComponentInstance *> &instances);

	VkPipeline pipeline;

	vk::GraphicsPipeline *graphicsPipeline;

	// Local Data
	typedef struct LocalDataEntry
	{
		UI_ComponentInstance *compInst;
		glm::vec2 position;
		glm::vec2 extent;
		glm::vec4 color;
		glm::vec4 scissor;
		glm::vec2 hz;
	} LocalDataEntry;

	typedef struct LocalData
	{
		vk::StagedBuffer localBuffer { VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT };

		vector<LocalDataEntry> entries;

		uint32_t numOpaque { 0 };
		uint32_t capacity { 1 };

		bool isLocalBufferDirty { true };
		bool isInitialized { false };
	} LocalData;

	map<UI_Context *, LocalData> localDataMap;

	void CreateLocalData(Renderer_Vulkan *r, UI_Context *ctx);
	void UploadLocalData(Renderer_Vulkan *r, LocalData &data);

	// Vertex Buffer
	VkBuffer vertexBufferObject;
	VkDeviceMemory vertexBufferMemory;
	VkDescriptorBufferInfo vertexBufferInfo;
#endif

	virtual void OnAddToContext(UI_ComponentInstance *compInst, UI_Context *ctx);
	virtual void OnRemoveFromContext(UI_ComponentInstance *compInst, UI_Context *ctx);
	
	virtual void OnMetricsUpdate(UI_ComponentInstance *compInst);
};

}
#endif
