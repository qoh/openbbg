#include "stdafx.h"

// OpenBBG
#include <OpenBBG/UI/UI_Context.h>

namespace openbbg {

UI_Context *g_masterContext = nullptr;

deque<UI_Context *> UI_Context::s_contextList;

}
