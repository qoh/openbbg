#ifndef _OPENBBG__UI__UI_COMPONENT_H_
#define _OPENBBG__UI__UI_COMPONENT_H_

// OpenBBG
#include <OpenBBG/Config.h>
#if OPENBBG_WITH_VULKAN
#include <OpenBBG/Renderer/Renderer_Vulkan.h>
#endif

namespace openbbg {

typedef struct UI_Context UI_Context;
typedef struct UI_Control UI_Control;
typedef struct UI_Component UI_Component;

struct UI_Component
{
	UI_Component();

	virtual ~UI_Component();
	
#if OPENBBG_WITH_VULKAN
	virtual void Init(Renderer_Vulkan *r) = 0;
	virtual void Cleanup(Renderer_Vulkan *r) = 0;
	virtual void Cleanup(Renderer_Vulkan *r, UI_Context *ctx) = 0;
	virtual void Cleanup(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl) = 0;
	static void CleanupAll(Renderer_Vulkan *r);
	virtual void Prepare(Renderer_Vulkan *r, UI_Context *ctx) = 0;
	virtual void Prepare(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl) = 0;
	virtual void RenderOpaque(Renderer_Vulkan *r, UI_Context *ctx) = 0;
	virtual void RenderTransparent(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl) = 0;
	virtual void RenderOverlay(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl) = 0;
#endif
};

}
#endif

// Definitions
#include <OpenBBG/UI/UI_Component_def.h>
