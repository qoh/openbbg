#pragma once

// OpenBBG
#include <OpenBBG/Renderer/Utility_Vulkan.h>

namespace openbbg {

typedef struct Window Window;
typedef struct Renderer_Vulkan Renderer_Vulkan;

struct Renderer_Vulkan
{
	Window *window;

	bool initialized;

	Renderer_Vulkan(Window *window);

	~Renderer_Vulkan();

	void Init();

	void Present();

	void Destroy();

	struct sample_info info;

	vk::GlobalInstance global;
};

}