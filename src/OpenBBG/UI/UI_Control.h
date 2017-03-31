#pragma once

namespace openbbg {

typedef struct UI_Control UI_Control;
typedef struct UI_ScissorRule UI_ScissorRule;

struct UI_ScissorRule
{
	/**
	 * Local scissor rectangle for clipping contents
	 */
	glm::vec4 scissorLocal;

	/**
	 * Absolute scissor rectangle for clipping contents, includes collisions with parent scissor spaces
	 */
	glm::vec4 scissorAbsolute;

	/**
	 * Scissor padding
	 */
	glm::vec4 scissorPadding;
};

struct UI_Control
{	
	UI_Control()
	{
	}

	~UI_Control()
	{
		for (auto child : children)
			delete child;
	}

	
	//------------------------ Metrics ---------------------------

	/**
	 * Position normalized by DPI scale
	 */
	glm::vec2 positionNormalized;

	/**
	 * Position relative to the local origin
	 */
	glm::vec2 positionLocal;

	/**
	 * Position relative to the absolute origin
	 */
	glm::vec2 positionAbsolute;

	/**
	 * Extent normalized by DPI scale
	 */
	glm::vec2 extentNormalized;

	/**
	 * Width and Height
	 */
	glm::vec2 extent;


	//------------------------ Family -------------------------
	
	/**
	 * Container of child controls
	 */
	deque<UI_Control *> children;

	/**
	 * Parent control
	 */
	UI_Control *parent { nullptr };

	/**
	 * Check if control is a child
	 * @param ctrl Target control
	 * @return true if target control is a child, otherwise false
	 */
	inline bool IsChild(UI_Control *ctrl)
	{
		// Do cheap check instead of searching through children
		return ctrl->parent == this;
	}

	/**
	 * Remove control from children
	 * @param ctrl Target control
	 */
	inline void RemoveChild(UI_Control *ctrl)
	{
		if (IsChild(ctrl) == false)
			return;
		children.erase(std::find(children.begin(), children.end(), ctrl));
		// TODO: uiClass->OnRemoveAsChild(ctrl, this);
		ctrl->parent = nullptr;
	}

	/**
	 * Add control to children
	 * @param ctrl Target control
	 */
	inline void AddChild(UI_Control *ctrl)
	{
		if (IsChild(ctrl))
			return;
		if (ctrl->parent != nullptr)
			ctrl->parent->RemoveChild(this);
		// UI_Control *oldParent { parent };
		ctrl->parent = this;
		// TODO: uiClass->OnAddAsChild(ctrl, ctrl->parent, oldParent);
		children.push_front(ctrl);
	}

	/**
	 * Move control to the front of the children
	 * @param ctrl Target control
	 */
	inline void MoveToFront(UI_Control *ctrl)
	{
		if (IsChild(ctrl) == false || children.front() == ctrl)
			return;
		children.erase(std::find(children.begin(), children.end(), ctrl));
		children.push_front(ctrl);
	}
	
	/**
	 * Move control to the back of the children
	 * @param ctrl Target control
	 */
	inline void MoveToBack(UI_Control *ctrl)
	{
		if (IsChild(ctrl) == false || children.back() == ctrl)
			return;
		children.erase(std::find(children.begin(), children.end(), ctrl));
		children.push_back(ctrl);
	}
	
	/**
	 * Swap control with its forward neighbor
	 * @param ctrl Target control
	 */
	inline void MoveForward(UI_Control *ctrl)
	{
		if (IsChild(ctrl) == false || children.front() == ctrl)
			return;
		auto search { std::find(children.begin(), children.end(), ctrl) };
		std::swap(*search, *(search - 1));
	}
	
	/**
	 * Swap control with its backward neighbor
	 * @param ctrl Target control
	 */
	inline void MoveBackward(UI_Control *ctrl)
	{
		if (IsChild(ctrl) == false || children.back() == ctrl)
			return;
		auto search { std::find(children.begin(), children.end(), ctrl) };
		std::swap(*search, *(search + 1));
	}


	//------------------------ Rules ---------------------------

	/**
	 * Scissor struct
	 */
	UI_ScissorRule *scissor { nullptr };

	
	//------------------------ Temp ---------------------------

	glm::vec4 color;

	// TODO: Components
	// TODO: Class
	// TODO: Profile
	// TODO: Zones
	// TODO: TilingRule
	// TODO: ResizeRule
	// TODO: PositionRule
	// TODO: DPI Detect
	// TODO: Transparency
	// TODO: Visibility
	// TODO: H-Z
};

}