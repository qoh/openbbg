#ifndef _OPENBBG__COMMON__DIRECTEDACYCLICGRAPHNODE_H_
#define _OPENBBG__COMMON__DIRECTEDACYCLICGRAPHNODE_H_

namespace openbbg {

template <class T>
struct DirectedAcyclicGraphEdge
{
	T *first;

	T *second;
};

template <class T, typename EdgeType>
struct DirectedAcyclicGraphNode
{
	vector<EdgeType *> inEdges;

	vector<EdgeType *> outEdges;

	EdgeType *GetInEdge(T *node);

	EdgeType *GetOutEdge(T *node);

	EdgeType *CreateEdge(T *node);

	void DeleteOutEdges();

	vector<T *> FlattenGraph();
};

}
#endif

// Definitions
#include <OpenBBG/Common/DirectedAcyclicGraphNode_def.h>
