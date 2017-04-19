#ifndef _OPENBBG__UI__UI_CLASS_CONTROL_H_
#define _OPENBBG__UI__UI_CLASS_CONTROL_H_

// OpenBBG
#include <OpenBBG/UI/UI_Class.h>

namespace openbbg {

typedef struct UI_Class_Control UI_Class_Control;

struct UI_Class_Control
	: UI_Class
	, Singleton<UI_Class_Control>
{
	UI_Class_Control();

	~UI_Class_Control();

	virtual UI_Control *Construct();

#if OPENBBG_WITH_VULKAN
	virtual void Init(Renderer_Vulkan *r);

	virtual void Cleanup(Renderer_Vulkan *r);

	virtual void Prepare(Renderer_Vulkan *r, UI_Control *ctrl);

	virtual void RenderOpaque(Renderer_Vulkan *r, UI_Control *ctrl);

	virtual void RenderTransparent(Renderer_Vulkan *r, UI_Control *ctrl);

	virtual void RenderOverlay(Renderer_Vulkan *r, UI_Control *ctrl);

	VkPipeline pipeline;

	vk::GraphicsPipeline *graphicsPipeline;

	// Vertex Buffer
	VkBuffer vertexBufferObject;
	VkDeviceMemory vertexBufferMemory;
	VkDescriptorBufferInfo vertexBufferInfo;
#endif
};

}
#endif

// Definitions
#include <OpenBBG/UI/UI_Class_Control_def.h>
