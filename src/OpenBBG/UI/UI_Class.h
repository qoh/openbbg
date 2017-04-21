#ifndef _OPENBBG__UI__UI_CLASS_H_
#define _OPENBBG__UI__UI_CLASS_H_

// OpenBBG
#include <OpenBBG/Config.h>
#include <OpenBBG/Common/Singleton.h>
#if OPENBBG_WITH_VULKAN
#include <OpenBBG/Renderer/Renderer_Vulkan.h>
#endif

namespace openbbg {

typedef struct UI_Context UI_Context;
typedef struct UI_Class UI_Class;
typedef struct UI_Control UI_Control;

struct UI_Class
{
	UI_Class();

	virtual ~UI_Class();

	map<UI_Context *, deque<UI_Control *>> controls;

	static vector<UI_Class *> s_classes;

	virtual UI_Control *Construct() = 0;
};

}
#endif

// Definitions
#include <OpenBBG/UI/UI_Class_def.h>
