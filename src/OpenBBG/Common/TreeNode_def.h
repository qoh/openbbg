#ifndef _OPENBBG__COMMON__TREENODE_DEF_H_
#define _OPENBBG__COMMON__TREENODE_DEF_H_

namespace openbbg {
	
template <class T>
inline
void
TreeNode<T>::AddChild(T *node)
{
	auto search = find(children.begin(), children.end(), node);
	if (search != children.end())
		return;
	children.push_back(node);
	static_cast<T *>(this)->OnChildAdded(node);
	static_cast<T *>(this)->OnChildrenChanged();
}

template <class T>
inline
void
TreeNode<T>::RemoveChild(T *node)
{
	auto search = find(children.begin(), children.end(), node);
	if (search == children.end())
		return;
	children.erase(search);
	static_cast<T *>(this)->OnChildRemoved(node);
	static_cast<T *>(this)->OnChildrenChanged();
}

template <class T>
inline
bool
TreeNode<T>::IsChild(T *node)
{
	return node->parent == this;
}

template <class T>
inline
void
TreeNode<T>::MoveChildToFront(T *node)
{
	if (IsChild(node) == false || children.front() == node)
		return;
	children.erase(find(children.begin(), children.end(), node));
	children.insert(children.begin(), node);
	static_cast<T *>(this)->OnChildrenChanged();
}

template <class T>
inline
void
TreeNode<T>::MoveChildToBack(T *node)
{
	if (IsChild(node) == false || children.back() == node)
		return;
	children.erase(find(children.begin(), children.end(), node));
	children.push_back(node);
	static_cast<T *>(this)->OnChildrenChanged();
}

template <class T>
inline
void
TreeNode<T>::MoveChildForward(T *node)
{
	if (IsChild(node) == false || children.front() == node)
		return;
	auto search { find(children.begin(), children.end(), node) };
	swap(*search, *(search - 1));
	static_cast<T *>(this)->OnChildrenChanged();
}

template <class T>
inline
void
TreeNode<T>::MoveChildBackward(T *node)
{
	if (IsChild(node) == false || children.back() == node)
		return;
	auto search { find(children.begin(), children.end(), node) };
	swap(*search, *(search + 1));
	static_cast<T *>(this)->OnChildrenChanged();
}

template <class T>
inline
void
TreeNode<T>::ClearChildren()
{
	while (children.empty() == false) {
		T *node = children.back();
		children.erase(children.end() - 1);
		static_cast<T *>(this)->OnChildRemoved(node);
	}
	static_cast<T *>(this)->OnChildrenChanged();
}

template <class T>
inline
void
TreeNode<T>::DeleteChildren()
{
	while (children.empty() == false) {
		T *node = children.back();
		children.erase(children.end() - 1);
		static_cast<T *>(this)->OnChildRemoved(node);
		delete node;
	}
	static_cast<T *>(this)->OnChildrenChanged();
}

template <class T>
inline
void
TreeNode<T>::OnChildAdded(T *child)
{
}
	
template <class T>
inline
void
TreeNode<T>::OnChildRemoved(T *child)
{
}
	
template <class T>
inline
void
TreeNode<T>::OnChildrenChanged()
{
}

}
#endif
