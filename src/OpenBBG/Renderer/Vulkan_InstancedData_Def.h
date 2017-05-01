#ifndef _OPENBBG__RENDERER__VULKAN_INSTANCEDDATA_DEF_H_
#define _OPENBBG__RENDERER__VULKAN_INSTANCEDDATA_DEF_H_

namespace openbbg {
namespace vk {

template <class T>
inline
void
InstancedData<T>::Init()
{
	if (isInitialized)
		return;

	// Store old capacity for logging purposes
	auto oldCapacity = capacity;

	if (capacity == 0)
		capacity = 1;
	uint32_t numInstances = (uint32_t)instances.size();
	while (capacity < numInstances)
		capacity <<= 1;

	uint32_t numBytes = capacity * sizeof(T);
	instanceBuffer.Init(numBytes);

	isInitialized = true;

	UpdateBuffers();
}

template <class T>
inline
void
InstancedData<T>::Cleanup()
{
	if (isInitialized == false)
		return;

	instanceBuffer.Cleanup();

	isInitialized = false;
}

template <class T>
inline
void
InstancedData<T>::UpdateBuffers()
{
	uint8_t *pData;
	instanceBuffer.MapMemory((void **)&pData);
	memcpy(pData, instances.data(), instances.size() * sizeof(T));
	instanceBuffer.UnmapMemory();

	instanceBuffer.CopyToDevice({ 0, 0, instances.size() * sizeof(T) });
}

template <class T>
inline
void
InstancedData<T>::Prepare(UI_Component *component, bool (*sortInstancesFunc)(T &a, T &b))
{
	if (isLocalBufferDirty) {
		// Resize if capacity is insufficient
		if (capacity < (uint32_t)instances.size()) {
			Cleanup();
			Init();
		}

		// Sort opaque/transparent
		sort(instances.begin(), instances.end(), sortInstancesFunc);

		// Update component instances
		{
			component->sortUpdateCallbackList.reserve(instances.size());
			uint32_t numEntries = (uint32_t)instances.size();
			for (uint32_t a = 0; a < numEntries; ++a) {
				auto &instance = instances[a];
				if (instance.compInst->instanceIdx != a || instance.compInst->isDirty) {
					component->sortUpdateCallbackList.push_back(instance.compInst);
					instance.compInst->instanceIdx = a;
					instance.compInst->isDirty = false;
				}
			}
		}

		// Minimize buffer updates
		{
			uint8_t *pData;
			instanceBuffer.MapMemory((void **)&pData);

			if (component->sortUpdateCallbackList.empty()) {
				memcpy(pData, instances.data(), instances.size() * sizeof(T));
			} else {
				uint32_t start;
				uint32_t last = numeric_limits<uint32_t>::max();
				for (auto instance : component->sortUpdateCallbackList) {
					if (last == numeric_limits<uint32_t>::max())
						start = instance->instanceIdx;
					else if ((instance->instanceIdx - last) > 1) {
						memcpy((T *)pData + start, instances.data() + start, (last - start + 1) * sizeof(T));
						start = instance->instanceIdx;
					}
					last = instance->instanceIdx;
				}

				if (last != numeric_limits<uint32_t>::max())
					memcpy((T *)pData + start, instances.data() + start, (last - start + 1) * sizeof(T));

				component->sortUpdateCallbackList.clear();
			}

			instanceBuffer.UnmapMemory();
		}

		instanceBuffer.CopyToDevice({ 0, 0, instances.size() * sizeof(T) });
		isLocalBufferDirty = false;
	}
}

}
}
#endif
