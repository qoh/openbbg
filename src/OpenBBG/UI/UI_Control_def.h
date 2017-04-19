#ifndef _OPENBBG__UI__UI_CONTROL_DEF_H_
#define _OPENBBG__UI__UI_CONTROL_DEF_H_

// OpenBBG
#include <OpenBBG/UI/UI_Context.h>
#include <OpenBBG/UI/UI_Class.h>

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
	DeleteChildren();
}

inline
void
UI_Control::OnChildAdded(UI_Control *child)
{
	child->context = context;
}

inline
void
UI_Control::OnChildRemoved(UI_Control *child)
{
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

	for (auto child : children)
		child->RemoveFromContext();
}

}
#endif
