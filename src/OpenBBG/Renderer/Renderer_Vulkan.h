#ifndef _OPENBBG__RENDERER__RENDERER_VULKAN_H_
#define _OPENBBG__RENDERER__RENDERER_VULKAN_H_

#define OPENBBG_VULKAN_VSYNC 0

// OpenBBG
#include <OpenBBG/Renderer/Utility_Vulkan.h>
#include <OpenBBG/Job.h>
#include <OpenBBG/Utility/Timing.h>

namespace openbbg {

typedef struct Window Window;
typedef struct Renderer_Vulkan Renderer_Vulkan;

struct Renderer_Vulkan
{
	static thread_local Renderer_Vulkan *s_current;

	static Renderer_Vulkan *Get();

	void SetCurrent();

	Window *window;

	bool isInitialized;

	bool isFirstFrame;
	
	// TEMP - Frame Time Logging
	TimeValue frameStart;
	TimeValue frameQueue;
	TimeValue frameEnd;
	AverageLog frameCPULog;
	AverageLog frameGPULog;

	Renderer_Vulkan(Window *window);

	~Renderer_Vulkan();

	void Init();

	void Render();

	void Destroy();

	void ResizeFramebuffer(int x, int y);

	vk::GlobalInstance global;
};

}
#endif

// Definitions
#include <OpenBBG/Renderer/Renderer_Vulkan_def.h>
