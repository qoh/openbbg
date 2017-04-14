#ifndef _OPENBBG__COMMON__TREENODE_H_
#define _OPENBBG__COMMON__TREENODE_H_

namespace openbbg {

template <class T>
struct TreeNode
{
	/**
	 * Container of child nodes
	 */
	vector<T *> children;

	/**
	 * Parent node
	 */
	T *parent { nullptr };

	/**
	 * Check if node is a child
	 * @param node Target node
	 * @return true if target node is a child, otherwise false
	 */
	bool IsChild(T *node);

	/**
	 * Remove node from children
	 * @param node Target node
	 */
	void RemoveChild(T *node);

	/**
	 * Add node to children
	 * @param node Target node
	 */
	void AddChild(T *node);

	/**
	 * Move node to the front of the children
	 * @param node Target node
	 */
	void MoveChildToFront(T *node);
	
	/**
	 * Move node to the back of the children
	 * @param node Target node
	 */
	void MoveChildToBack(T *node);
	
	/**
	 * Swap node with its forward neighbor
	 * @param node Target node
	 */
	void MoveChildForward(T *node);
	
	/**
	 * Swap node with its backward neighbor
	 * @param node Target node
	 */
	void MoveChildBackward(T *node);
	
	/**
	 * Remove and delete all children
	 */
	void DeleteChildren();
	
	/**
	 * Remove all children
	 */
	void ClearChildren();

	/**
	 * Callback for when a child is added
	 */
	void OnChildAdded(T *child);
	
	/**
	 * Callback for when a child is removed
	 */
	void OnChildRemoved(T *child);
	
	/**
	 * Callback for when the children are changed
	 */
	void OnChildrenChanged();
};

}
#endif

// Definitions
#include <OpenBBG/Common/TreeNode_def.h>
