#pragma once

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
	Window *window;

	bool isInitialized;

	bool isFirstFrame;
	
	// TEMP - Frame Time Logging (No VSync)
#if !OPENBBG_VULKAN_VSYNC
	TimeValue frameStart;
	TimeValue frameQueue;
	TimeValue frameEnd;
	AverageLog frameCPULog;
	AverageLog frameGPULog;
#endif

	Renderer_Vulkan(Window *window);

	~Renderer_Vulkan();

	void Init();

	void Render();

	void Destroy();

	void ResizeFramebuffer(int x, int y);

	struct sample_info info;

	vk::GlobalInstance global;
};

}