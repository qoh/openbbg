#ifndef _OPENBBG__RENDERER__RENDERNODE_DEF_H_
#define _OPENBBG__RENDERER__RENDERNODE_DEF_H_

namespace openbbg {
	
inline
RenderNode::RenderNode(const char *name)
	: DirectedAcyclicGraphNode<RenderNode, DirectedAcyclicGraphEdge<RenderNode>>()
	, name(name)
{
}

inline
RenderNode::~RenderNode()
{
	DeleteOutEdges();
}

}
#endif
