#ifndef _OPENBBG__COMMON__DIRECTEDGRAPHNODE_DEF_H_
#define _OPENBBG__COMMON__DIRECTEDGRAPHNODE_DEF_H_

namespace openbbg {
	
	
template <class T, typename EdgeType>
inline
EdgeType *
DirectedAcyclicGraphNode<T, EdgeType>::GetInEdge(T *node)
{
	for (auto edge : inEdges)
		if (edge->first == node)
			return edge;
	return nullptr;
}

template <class T, typename EdgeType>
inline
EdgeType *
DirectedAcyclicGraphNode<T, EdgeType>::GetOutEdge(T *node)
{
	for (auto edge : outEdges)
		if (edge->second == node)
			return edge;
	return nullptr;
}

template <class T, typename EdgeType>
inline
void
DirectedAcyclicGraphNode<T, EdgeType>::DeleteOutEdges()
{
	for (auto edge : outEdges)
		delete edge;
	outEdges.clear();
}

template <class T, typename EdgeType>
inline
EdgeType *
DirectedAcyclicGraphNode<T, EdgeType>::CreateEdge(T *node)
{
	EdgeType *edge = new EdgeType();
	edge->first = static_cast<T *>(this);
	edge->second = node;
	outEdges.push_back(edge);
	node->inEdges.push_back(edge);
	return edge;
}


template <class T, typename EdgeType>
inline
vector<T *>
DirectedAcyclicGraphNode<T, EdgeType>::FlattenGraph()
{
	// Kahn's Algorithm
	// REF: https://en.wikipedia.org/wiki/Topological_sorting

	map<EdgeType *, bool> traversalMap;
	vector<T *> sorted;
	deque<T *> S;

	// We assume this starting node has no incoming edges
	S.push_back(static_cast<T *>(this));

	while (S.empty() == false) {
		auto n = S.front();
		S.pop_front();
		sorted.push_back(n);
		for (auto e : n->outEdges) {
			traversalMap[e] = true;
			auto m = e->second;
			bool noMoreIn = true;
			for (auto in : m->inEdges) {
				auto search = traversalMap.find(in);
				if (search == traversalMap.end()) {
					noMoreIn = false;
					break;
				}
			}
			if (noMoreIn)
				S.push_back(m);
		}
	}

	return sorted;
}

}
#endif
