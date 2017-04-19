#ifndef _OPENBBG__UI__UI_CLASS_H_
#define _OPENBBG__UI__UI_CLASS_H_

// OpenBBG
#include <OpenBBG/Config.h>
#include <OpenBBG/Common/Singleton.h>
#if OPENBBG_WITH_VULKAN
#include <OpenBBG/Renderer/Renderer_Vulkan.h>
#endif

namespace openbbg {

typedef struct UI_Context UI_Context;
typedef struct UI_Class UI_Class;
typedef struct UI_Control UI_Control;

struct UI_Class
{
	UI_Class();

	virtual ~UI_Class();

	uint32_t isInitialized : 1;

	map<UI_Context *, deque<UI_Control *>> controls;

	map<UI_Context *, deque<UI_Control *>> controlsOpaque;

	map<UI_Context *, deque<UI_Control *>> controlsTransparent;

	map<UI_Context *, deque<UI_Control *>> controlsOverlay;

	virtual UI_Control *Construct() = 0;
	
#if OPENBBG_WITH_VULKAN
	virtual void Init(Renderer_Vulkan *r) = 0;
	virtual void Cleanup(Renderer_Vulkan *r) = 0;
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
#include <OpenBBG/UI/UI_Class_def.h>
