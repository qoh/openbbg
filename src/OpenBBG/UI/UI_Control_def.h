#ifndef _OPENBBG__UI__UI_CONTROL_DEF_H_
#define _OPENBBG__UI__UI_CONTROL_DEF_H_

// OpenBBG
#include <OpenBBG/UI/UI_Context.h>

namespace openbbg {

inline UI_Control::UI_Control()
{
}

inline UI_Control::~UI_Control()
{
	for (auto child : children)
		delete child;
}

inline void UI_Control::AddChild(UI_Control *ctrl)
{
	if (IsChild(ctrl))
		return;
	if (ctrl->parent != nullptr)
		ctrl->parent->RemoveChild(this);
	// UI_Control *oldParent { parent };
	ctrl->parent = this;
	// TODO: uiClass->OnAddAsChild(ctrl, ctrl->parent, oldParent);
	children.push_front(ctrl);
}

inline void UI_Control::MoveToFront(UI_Control *ctrl)
{
	if (IsChild(ctrl) == false || children.front() == ctrl)
		return;
	children.erase(std::find(children.begin(), children.end(), ctrl));
	children.push_front(ctrl);
}

inline void UI_Control::MoveToBack(UI_Control *ctrl)
{
	if (IsChild(ctrl) == false || children.back() == ctrl)
		return;
	children.erase(std::find(children.begin(), children.end(), ctrl));
	children.push_back(ctrl);
}

inline void UI_Control::MoveForward(UI_Control *ctrl)
{
	if (IsChild(ctrl) == false || children.front() == ctrl)
		return;
	auto search { std::find(children.begin(), children.end(), ctrl) };
	std::swap(*search, *(search - 1));
}

inline void UI_Control::MoveBackward(UI_Control *ctrl)
{
	if (IsChild(ctrl) == false || children.back() == ctrl)
		return;
	auto search { std::find(children.begin(), children.end(), ctrl) };
	std::swap(*search, *(search + 1));
}


}
#endif
