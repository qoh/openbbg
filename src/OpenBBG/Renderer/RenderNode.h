#ifndef _OPENBBG__RENDERER__RENDERNODE_H_
#define _OPENBBG__RENDERER__RENDERNODE_H_

// OpenBBG
#include <OpenBBG/Common/DirectedAcyclicGraphNode.h>
#include <OpenBBG/Renderer/Utility_Vulkan.h>

namespace openbbg {

typedef struct RenderNode RenderNode;

struct RenderNode : DirectedAcyclicGraphNode<RenderNode, DirectedAcyclicGraphEdge<RenderNode>>
{
	string name;

	RenderNode(const char *name);

	~RenderNode();
};

}
#endif

// Definitions
#include <OpenBBG/Renderer/RenderNode_def.h>
