#ifndef _OPENBBG__RENDERER__RENDERER_VULKAN_DEF_H_
#define _OPENBBG__RENDERER__RENDERER_VULKAN_DEF_H_

namespace openbbg {
	
inline
Renderer_Vulkan *
Renderer_Vulkan::Get()
{
	return s_current;
}

}
#endif
