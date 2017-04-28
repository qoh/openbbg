#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Modules/Module_Lua.h>
#include <OpenBBG/Modules/Module_UI.h>
#include <OpenBBG/Game.h>
#include <OpenBBG/UI/UI_Context.h>
#include <OpenBBG/UI/UI_Control.h>
#include <OpenBBG/UI/UI_Classes.h>
#include <OpenBBG/UI/UI_Components.h>

namespace openbbg {

IMPLEMENT_MODULE(Module_UI, Phase_Startup, true)
MODULE_DEPENDENCY(Module_UI, Module_Lua, true)

bool Module_UI::ModuleInit(Game *game)
{
	// Test UI
#if 1
	UI_Context *ctx = new UI_Context();
	UI_Control *root = UI_Class_ColorCtrl::Get()->Construct();
	{
		auto compInst = static_cast<UI_ComponentInstance_ColorQuad *>(root->componentInstances[0]);
		compInst->color = { 0.f, 0.f, 1.f, 1.f };
		compInst->relativePosition = { 128.f, 128.f };
		compInst->extent = { 256.f, 128.f };
		compInst->zActual = -0.5f;
	}
	
	ctx->SetRoot(root);
	uint32_t numRandom = 15;
	glm::vec2 screen { 1280.f, 720.f };
	glm::vec2 ext { 32.f, 32.f };
	srand((unsigned int)time(NULL));
	for (uint32_t a = 0; a < numRandom; ++a) {
		UI_Control *child = UI_Class_ColorCtrl::Get()->Construct();
		{
			auto compInst = static_cast<UI_ComponentInstance_ColorQuad *>(child->componentInstances[0]);
			compInst->relativePosition = { static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (screen.x - ext.x), static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (screen.y - ext.y) };
			compInst->extent = ext;
//			compInst->color = { static_cast<float>(rand()) / static_cast<float>(RAND_MAX), static_cast<float>(rand()) / static_cast<float>(RAND_MAX), static_cast<float>(rand()) / static_cast<float>(RAND_MAX), (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) < 0.15f ? 1.f : static_cast<float>(rand()) / static_cast<float>(RAND_MAX) };
			compInst->color = { static_cast<float>(rand()) / static_cast<float>(RAND_MAX), static_cast<float>(rand()) / static_cast<float>(RAND_MAX), static_cast<float>(rand()) / static_cast<float>(RAND_MAX), 0.9f };
//			compInst->color = { static_cast<float>(rand()) / static_cast<float>(RAND_MAX), static_cast<float>(rand()) / static_cast<float>(RAND_MAX), static_cast<float>(rand()) / static_cast<float>(RAND_MAX), 1.f };
			compInst->zActual = -static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
//			compInst->zActual = -0.15f;
		}
		root->AddChild(child);
	}

	g_masterContext = ctx;
#endif
	Module_Lua::Get()->ExecuteScript("assets/scripts/ui-test.lua");

	return true;
}

void Module_UI::ModuleCleanup(Game *game)
{
	UI_Context::CleanupAll();
}

}
