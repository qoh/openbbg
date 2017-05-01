#ifndef _OPENBBG__RENDERER__VULKAN_INSTANCEDDATA_H_
#define _OPENBBG__RENDERER__VULKAN_INSTANCEDDATA_H_

namespace openbbg {
namespace vk {

template <class T>
struct InstancedData
{
	vk::StagedBuffer instanceBuffer { VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT };

	vector<T> instances;

	uint32_t numOpaque { 0 };
	uint32_t capacity { 1 };

	bool isLocalBufferDirty { true };
	bool isInitialized { false };

	void Init();

	void Cleanup();

	void UpdateBuffers();

	void Prepare(UI_Component *component, bool (*sortInstancesFunc)(T &a, T &b));
};

}
}
#endif

// Definitions
#include <OpenBBG/Renderer/Vulkan_InstancedData_def.h>
