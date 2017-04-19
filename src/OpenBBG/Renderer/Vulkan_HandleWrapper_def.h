#ifndef _OPENBBG__RENDERER__VULKAN_HANDLEWRAPPER_DEF_H_
#define _OPENBBG__RENDERER__VULKAN_HANDLEWRAPPER_DEF_H_

namespace openbbg {

template <typename T>
inline
VkWrap<T>::VkWrap(T &inHandle)
	: handle(inHandle)
{
}

template <typename T>
inline
const T *
VkWrap<T>::operator &() const
{
	return &handle;
}

template <typename T>
inline
VkWrap<T>::operator T() const
{
	return handle;
}

template <typename T>
inline
void
VkWrap<T>::operator=(T input)
{
	if (input != handle) {
		if (handle != VK_NULL_HANDLE)
			Cleanup();
		handle = input;
	}
}

template <typename T>
inline
T *
VkWrap<T>::Replace()
{
	if (handle != VK_NULL_HANDLE)
		Cleanup();
	return &handle;
}

template <typename T>
inline
VkWrap<T>::~VkWrap()
{
	if (handle != VK_NULL_HANDLE)
		Cleanup();
}

//------------------------------------

template <>
inline
void
VkWrap<VkBuffer>::Cleanup()
{
	vkDestroyBuffer(global.device, info.vertex_buffer.buf, NULL);
	vkFreeMemory(global.device, info.vertex_buffer.mem, NULL);
}

}
#endif
