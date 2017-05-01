#ifndef _OPENBBG__UI__UI_COMPONENT_COLORQUAD_H_
#define _OPENBBG__UI__UI_COMPONENT_COLORQUAD_H_

// OpenBBG
#include <OpenBBG/UI/UI_Component.h>
#include <OpenBBG/Renderer/Vulkan_Buffer.h>
#include <OpenBBG/Renderer/Vulkan_StagedBuffer.h>
#include <OpenBBG/Renderer/Vulkan_InstancedData.h>
#include <OpenBBG/Renderer/Vulkan_Pipeline.h>

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
	virtual void Init();

	virtual void Cleanup();

	virtual void Cleanup(UI_Context *ctx);

	virtual void Cleanup(UI_Context *ctx, UI_ComponentInstance *compInst);

	virtual void Prepare(UI_Context *ctx);

	virtual void Prepare(UI_Context *ctx, UI_ComponentInstance *compInst);

	virtual void RenderOpaque(UI_Context *ctx);

	virtual void RenderTransparent(UI_Context *ctx, vector<UI_ComponentInstance *> &instances, uint32_t startInstance, uint32_t numInstances);

	virtual void RenderOverlay(UI_Context *ctx, UI_ComponentInstance *compInst);

	virtual void PopulateTransparentInstances(UI_Context *ctx, vector<UI_ComponentInstance *> &instances);

	VkPipeline pipeline;

	vk::GraphicsPipeline *graphicsPipeline;

	// Local Data
	typedef struct Instance
	{
		UI_ComponentInstance *compInst;
		glm::vec2 position;
		glm::vec2 extent;
		glm::vec4 color;
		glm::vec4 scissor;
		glm::vec2 hz;
	} Instance;

	typedef vk::InstancedData<Instance> InstancedData;

	map<UI_Context *, InstancedData> instancedDataMap;

	void CreateInstancedData(UI_Context *ctx);

	vk::Buffer vertexBuffer { VK_BUFFER_USAGE_VERTEX_BUFFER_BIT };
#endif

	virtual void OnAddToContext(UI_ComponentInstance *compInst, UI_Context *ctx);
	virtual void OnRemoveFromContext(UI_ComponentInstance *compInst, UI_Context *ctx);
	
	virtual void OnMetricsUpdate(UI_ComponentInstance *compInst);
};

}
#endif
