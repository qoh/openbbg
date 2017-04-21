#ifndef _OPENBBG__UI__UI_CONTROL_DEF_H_
#define _OPENBBG__UI__UI_CONTROL_DEF_H_

// OpenBBG
#include <OpenBBG/UI/UI_Context.h>
#include <OpenBBG/UI/UI_Class.h>
#include <OpenBBG/UI/UI_Component.h>

namespace openbbg {

inline
UI_Control::UI_Control()
	: TreeNode<UI_Control>()
	, uiClass(nullptr)
	, context(nullptr)
{
}

inline
UI_Control::~UI_Control()
{
	for (auto compInst : componentInstances)
		delete compInst;

	DeleteChildren();
}

inline
void
UI_Control::OnChildAdded(UI_Control *child)
{
	child->context = context;
	if (context != nullptr)
		child->AddToContext();
}

inline
void
UI_Control::OnChildRemoved(UI_Control *child)
{
	if (context != nullptr)
		child->RemoveFromContext();
	child->context = nullptr;
}

inline
void
UI_Control::OnChildrenChanged()
{
}

inline
void
UI_Control::AddToContext()
{
	auto &controls = uiClass->controls[context];
	if (controls.empty())
		context->classes.push_back(uiClass);
	controls.push_back(this);

	// Add components
	for (auto compInst : componentInstances) {
		auto &compInsts = compInst->component->componentInstances[context];
		if (compInsts.empty())
			context->components.push_back(compInst->component);
		compInsts.push_back(compInst);
	}

	for (auto child : children)
		child->AddToContext();
}

inline
void
UI_Control::RemoveFromContext()
{
	auto &controls = uiClass->controls[context];
	controls.erase(find(controls.begin(), controls.end(), this));
	if (controls.empty()) {
		auto &classes = context->classes;
		classes.erase(find(classes.begin(), classes.end(), uiClass));
		uiClass->controls.erase(context);
	}

	// Remove components
	for (auto compInst : componentInstances) {
		auto &compInsts = compInst->component->componentInstances[context];
		compInsts.erase(find(compInsts.begin(), compInsts.end(), compInst));
		if (compInsts.empty()) {
			auto &components = context->components;
			components.erase(find(components.begin(), components.end(), compInst->component));
			compInst->component->componentInstances.erase(context);
		}
	}

	for (auto child : children)
		child->RemoveFromContext();
}

}
#endif
