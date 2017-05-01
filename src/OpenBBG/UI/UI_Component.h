#ifndef _OPENBBG__UI__UI_COMPONENT_H_
#define _OPENBBG__UI__UI_COMPONENT_H_

// OpenBBG
#include <OpenBBG/Config.h>
#include <OpenBBG/Common/Singleton.h>
#if OPENBBG_WITH_VULKAN
#include <OpenBBG/Renderer/Renderer_Vulkan.h>
#endif

namespace openbbg {

typedef struct UI_Context UI_Context;
typedef struct UI_Control UI_Control;
typedef struct UI_Component UI_Component;
typedef struct UI_ComponentInstance UI_ComponentInstance;

struct UI_ComponentInstance
{
	UI_Component *component;
	UI_Control *control;
	uint32_t instanceIdx { std::numeric_limits<uint32_t>::max() };
	bool isDirty { true };

	float zOffset;

	// TODO: Alignment Rules
	glm::vec2 relativePosition;
	glm::vec2 extent;
	float zActual;
};

struct UI_Component
{
	UI_Component();

	virtual ~UI_Component();

	uint32_t isInitialized : 1;

	vector<UI_ComponentInstance *> sortUpdateCallbackList;

	map<UI_Context *, deque<UI_ComponentInstance *>> componentInstances;

	map<UI_Context *, deque<UI_ComponentInstance *>> componentInstancesOpaque;

	map<UI_Context *, deque<UI_ComponentInstance *>> componentInstancesTransparent;

	map<UI_Context *, deque<UI_ComponentInstance *>> componentInstancesOverlay;

	static vector<UI_Component *> s_components;

	static UI_Component *s_lastComponentRendered;

	virtual UI_ComponentInstance *Construct() = 0;

	virtual void Deconstruct(UI_ComponentInstance *compInst) = 0;
	
#if OPENBBG_WITH_VULKAN
	static void CleanupAll();
	virtual void Init() = 0;
	virtual void Cleanup() = 0;
	virtual void Cleanup(UI_Context *ctx) = 0;
	virtual void Cleanup(UI_Context *ctx, UI_ComponentInstance *compInst) = 0;
	virtual void Prepare(UI_Context *ctx) = 0;
	virtual void RenderOpaque(UI_Context *ctx) = 0;
	virtual void RenderTransparent(UI_Context *ctx, vector<UI_ComponentInstance *> &instances, uint32_t startInstance, uint32_t numInstances) = 0;
	virtual void RenderOverlay(UI_Context *ctx, UI_ComponentInstance *compInst) = 0;
	virtual void Prepare(UI_Context *ctx, UI_ComponentInstance *compInst) = 0;
	virtual void PopulateTransparentInstances(UI_Context *ctx, vector<UI_ComponentInstance *> &instances) = 0;
#endif

	virtual void OnAddToContext(UI_ComponentInstance *compInst, UI_Context *ctx) = 0;
	virtual void OnRemoveFromContext(UI_ComponentInstance *compInst, UI_Context *ctx) = 0;

	virtual void OnMetricsUpdate(UI_ComponentInstance *compInst) = 0;
};

}
#endif

// Definitions
#include <OpenBBG/UI/UI_Component_def.h>
