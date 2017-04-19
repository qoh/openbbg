#include "stdafx.h"

// OpenBBG
#include <OpenBBG/UI/UI_Class_Control.h>
#include <OpenBBG/UI/UI_Control.h>

namespace openbbg {

IMPLEMENT_SINGLETON(UI_Class_Control);

UI_Class_Control::UI_Class_Control()
	: UI_Class()
{
}

UI_Class_Control::~UI_Class_Control()
{
}

UI_Control *
UI_Class_Control::Construct()
{
	auto ctrl = new UI_Control();
	ctrl->uiClass = this;
	return ctrl;
}

#if OPENBBG_WITH_VULKAN
void
UI_Class_Control::Init(Renderer_Vulkan *r)
{
	if (isInitialized)
		return;

	isInitialized = true;
}

void
UI_Class_Control::Cleanup(Renderer_Vulkan *r)
{
	if (isInitialized == false)
		return;
	
	isInitialized = false;
}

void
UI_Class_Control::Prepare(Renderer_Vulkan *r, UI_Context *ctx)
{
	if (isInitialized == false)
		Init(r);
}

void
UI_Class_Control::Prepare(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl)
{
}

void
UI_Class_Control::RenderOpaque(Renderer_Vulkan *r, UI_Context *ctx)
{
}

void
UI_Class_Control::RenderTransparent(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl)
{
}

void
UI_Class_Control::RenderOverlay(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl)
{
}
#endif

}
