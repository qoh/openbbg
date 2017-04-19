#ifndef _OPENBBG__UI__UI_CONTEXT_H_
#define _OPENBBG__UI__UI_CONTEXT_H_

// OpenBBG
#include <OpenBBG/Config.h>
#if OPENBBG_WITH_VULKAN
#include <OpenBBG/Renderer/Renderer_Vulkan.h>
#endif

namespace openbbg {

typedef struct UI_Class UI_Class;
typedef struct UI_Context UI_Context;
typedef struct UI_Control UI_Control;

struct UI_Context
{
	/**
	 * Extent normalized by DPI scale
	 */
	glm::vec2 extentNormalized;

	/**
	 * Width and Height
	 */
	glm::vec2 extent;
	
	
	/**
	 * Root control
	 */
	UI_Control *root;


	UI_Context();

	~UI_Context();


	//-----------------------------------
	
#if OPENBBG_WITH_VULKAN
	void Prepare(Renderer_Vulkan *r);
	void Render(Renderer_Vulkan *r);
	void Cleanup(Renderer_Vulkan *r);
#endif

	//--------------------------------------

	deque<UI_Class *> classes;

	void SetRoot(UI_Control *ctrl);

	static deque<UI_Context *> s_contextList;

	static void CleanupAll();
};

extern UI_Context *g_masterContext;

}
#endif

// Definitions
#include <OpenBBG/UI/UI_Context_def.h>
