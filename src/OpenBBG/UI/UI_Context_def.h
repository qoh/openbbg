#ifndef _OPENBBG__UI__UI_CONTEXT_DEF_H_
#define _OPENBBG__UI__UI_CONTEXT_DEF_H_

// OpenBBG
#include <OpenBBG/UI/UI_Control.h>
#include <OpenBBG/UI/UI_Component.h>
#include <OpenBBG/UI/UI_Class.h>

namespace openbbg {

inline
UI_Context::UI_Context()
	: root { nullptr }
{
	s_contextList.push_back(this);
}

inline
UI_Context::~UI_Context()
{
	if (root != nullptr)
		delete root;
	s_contextList.erase(find(s_contextList.begin(), s_contextList.end(), this));
}

inline
void
UI_Context::SetRoot(UI_Control *ctrl)
{
	// Reset context references if root exists
	if (root != nullptr && ctrl != root) {
		root->RemoveFromContext();

		deque<UI_Control *> q;
		q.push_back(ctrl);
		while (q.empty() == false) {
			UI_Control *c = q.front();
			q.pop_front();
			c->context = this;
			for (auto child : c->children)
				q.push_back(child);
		}
	}

	root = ctrl;

	// Update context references
	if (ctrl != nullptr) {
		deque<UI_Control *> q;
		q.push_back(ctrl);
		while (q.empty() == false) {
			UI_Control *c = q.front();
			q.pop_front();
			c->context = this;
			for (auto child : c->children)
				q.push_back(child);
		}

		ctrl->AddToContext();
	}
}

inline
void
UI_Context::CleanupAll()
{
	while (s_contextList.empty() == false)
		delete s_contextList.back();
}

//-----------------------

#if OPENBBG_WITH_VULKAN
inline
void
UI_Context::Prepare(Renderer_Vulkan *r)
{
	// Prepare
	for (auto uiComponent : components) {
		uiComponent->Prepare(r, this);
		for (auto compInst : uiComponent->componentInstances[this])
			uiComponent->Prepare(r, this, compInst);
	}
}

inline
bool
sortTransparentComponentInstances(UI_ComponentInstance *a, UI_ComponentInstance *b)
{
	return a->zActual < b->zActual;
}

inline
void
UI_Context::Render(Renderer_Vulkan *r)
{
	// TODO: Properly distinguish the different groups of renderables

	// Render all opaque
	for (auto uiComponent : components)
		uiComponent->RenderOpaque(r, this);
	
	// Render all transparent
	if (isTransparentInstancesDirty) {
		transparentInstances.clear();
		for (auto uiComponent : components)
			uiComponent->PopulateTransparentInstances(r, this, transparentInstances);
		sort(transparentInstances.begin(), transparentInstances.end(), sortTransparentComponentInstances);
		isTransparentInstancesDirty = false;
	}
	UI_Component::s_lastComponentRendered = nullptr;
	uint32_t numInstances = (uint32_t)transparentInstances.size();
#if 1
	UI_Component *lastComponent = nullptr;
	uint32_t startInstanceIdx = 0;
	uint32_t numInstancesM1 = numInstances - 1;
	for (uint32_t a = 0; a < numInstances; ++a) {
		auto compInst = transparentInstances[a];
		if (lastComponent == compInst->component && a != numInstancesM1)
			continue; //  && ((a == numInstancesM1 && compInst->component != lastComponent) || a != numInstancesM1)
		if (lastComponent != nullptr) {
			if (compInst->component == lastComponent) {
				lastComponent->RenderTransparent(r, this, transparentInstances, startInstanceIdx, a - startInstanceIdx + 1);
				continue;
			}
			lastComponent->RenderTransparent(r, this, transparentInstances, startInstanceIdx, a - startInstanceIdx);
		}
		startInstanceIdx = a;
		lastComponent = compInst->component;
		if (a == numInstancesM1)
			lastComponent->RenderTransparent(r, this, transparentInstances, startInstanceIdx, a - startInstanceIdx + 1);
	}
#else
	for (uint32_t a = 0; a < numInstances; ++a) {
		auto compInst = transparentInstances[a];
		compInst->component->RenderTransparent(r, this, transparentInstances, a, 1);
		UI_Component::s_lastComponentRendered = compInst->component;
	}
#endif

	// Clear depth

	// Render all overlay
	for (auto uiComponent : components)
		for (auto compInst : uiComponent->componentInstances[this])
			uiComponent->RenderOverlay(r, this, compInst);
}

inline
void
UI_Context::Cleanup(Renderer_Vulkan *r)
{
	// Cleanup
	for (auto uiComponent : components) {
		for (auto compInst : uiComponent->componentInstances[this])
			uiComponent->Cleanup(r, this, compInst);
		uiComponent->Cleanup(r, this);
	}
}
#endif

}
#endif
