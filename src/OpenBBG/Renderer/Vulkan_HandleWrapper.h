#ifndef _OPENBBG__RENDERER__VULKAN_HANDLEWRAPPER_H_
#define _OPENBBG__RENDERER__VULKAN_HANDLEWRAPPER_H_

namespace openbbg {
	
template <typename T>
struct VkWrap
{
	T handle { VK_NULL_HANDLE };

	void Cleanup();

	VkWrap(T &inHandle);

	const T *operator &() const;

	operator T() const;

	void operator=(T input);

	T *Replace();

	~VkWrap();
};

}
#endif

// Definitions
#include <OpenBBG/Renderer/Vulkan_HandleWrapper_def.h>
