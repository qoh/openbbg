#include "stdafx.h"

// OpenBBG
#include <OpenBBG/UI/UI_Component.h>

namespace openbbg {

vector<UI_Component *> UI_Component::s_components;

UI_Component::UI_Component()
	: isInitialized(false)
{
	s_components.push_back(this);
}

UI_Component::~UI_Component()
{
	s_components.erase(find(s_components.begin(), s_components.end(), this));
}

#if OPENBBG_WITH_VULKAN
void
UI_Component::CleanupAll(Renderer_Vulkan *r)
{
	for (auto cl : s_components)
		if (cl->isInitialized)
			cl->Cleanup(r);
}
#endif

}
