#ifndef _OPENBBG__UI__UI_CONTEXT_DEF_H_
#define _OPENBBG__UI__UI_CONTEXT_DEF_H_

// OpenBBG
#include <OpenBBG/UI/UI_Control.h>

namespace openbbg {

inline UI_Context::UI_Context()
	: root { nullptr }
{
	s_contextList.push_back(this);
}

inline UI_Context::~UI_Context()
{
	if (root != nullptr)
		delete root;
	s_contextList.erase(find(s_contextList.begin(), s_contextList.end(), this));
}


inline void UI_Context::SetRoot(UI_Control *ctrl)
{
	root = ctrl;
}

inline void UI_Context::CleanupAll()
{
	while (s_contextList.empty() == false)
		delete s_contextList.back();
}

}
#endif
