#ifndef _OPENBBG__UI__UI_CONTEXT_DEF_H_
#define _OPENBBG__UI__UI_CONTEXT_DEF_H_

// OpenBBG
#include <OpenBBG/UI/UI_Control.h>
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
	for (auto uiClass : classes) {
		uiClass->Prepare(r, this);
		for (auto ctrl : uiClass->controls[this])
			uiClass->Prepare(r, this, ctrl);
	}
}

inline
void
UI_Context::Render(Renderer_Vulkan *r)
{
	// Render all opaque
	for (auto uiClass : classes)
		uiClass->RenderOpaque(r, this);

	// Render all transparent
	for (auto uiClass : classes)
		for (auto ctrl : uiClass->controls[this])
			uiClass->RenderTransparent(r, this, ctrl);

	// Clear depth

	// Render all overlay
	for (auto uiClass : classes)
		for (auto ctrl : uiClass->controls[this])
			uiClass->RenderOverlay(r, this, ctrl);
}
#endif

}
#endif
