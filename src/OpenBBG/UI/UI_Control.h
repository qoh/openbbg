#ifndef _OPENBBG__UI__UI_CONTROL_H_
#define _OPENBBG__UI__UI_CONTROL_H_

namespace openbbg {

typedef struct UI_Context UI_Context;
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
	UI_Control();

	~UI_Control();

	
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
	bool IsChild(UI_Control *ctrl);

	/**
	 * Remove control from children
	 * @param ctrl Target control
	 */
	void RemoveChild(UI_Control *ctrl);

	/**
	 * Add control to children
	 * @param ctrl Target control
	 */
	void AddChild(UI_Control *ctrl);

	/**
	 * Move control to the front of the children
	 * @param ctrl Target control
	 */
	void MoveToFront(UI_Control *ctrl);
	
	/**
	 * Move control to the back of the children
	 * @param ctrl Target control
	 */
	void MoveToBack(UI_Control *ctrl);
	
	/**
	 * Swap control with its forward neighbor
	 * @param ctrl Target control
	 */
	void MoveForward(UI_Control *ctrl);
	
	/**
	 * Swap control with its backward neighbor
	 * @param ctrl Target control
	 */
	void MoveBackward(UI_Control *ctrl);


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
#endif

// Definitions
#include <OpenBBG/UI/UI_Control_def.h>
