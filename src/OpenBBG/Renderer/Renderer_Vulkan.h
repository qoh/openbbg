#pragma once

namespace openbbg {

typedef struct Window Window;
typedef struct Renderer_Vulkan Renderer_Vulkan;

struct Renderer_Vulkan
{
	Window *window;

	Renderer_Vulkan(Window *window);

	~Renderer_Vulkan();
};

}