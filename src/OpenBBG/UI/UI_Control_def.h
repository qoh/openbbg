#ifndef _OPENBBG__UI__UI_CONTROL_DEF_H_
#define _OPENBBG__UI__UI_CONTROL_DEF_H_

// OpenBBG
#include <OpenBBG/UI/UI_Context.h>

namespace openbbg {

inline
UI_Control::UI_Control()
	: TreeNode<UI_Control>()
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
	LOG_INFO("Added child");
}

inline
void
UI_Control::OnChildRemoved(UI_Control *child)
{
	LOG_INFO("Removed child");
}

inline
void
UI_Control::OnChildrenChanged()
{
}

}
#endif
