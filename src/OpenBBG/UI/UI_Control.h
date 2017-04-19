#ifndef _OPENBBG__UI__UI_CONTROL_H_
#define _OPENBBG__UI__UI_CONTROL_H_

// OpenBBG
#include <OpenBBG/Common/TreeNode.h>

namespace openbbg {

typedef struct UI_Class UI_Class;
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

struct UI_Control : TreeNode<UI_Control>
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
	
	void OnChildAdded(UI_Control *child);
	
	void OnChildRemoved(UI_Control *child);
	
	void OnChildrenChanged();


	//------------------------ Rules ---------------------------

	/**
	 * Scissor struct
	 */
	UI_ScissorRule *scissor { nullptr };

	
	//------------------------ Temp ---------------------------

	glm::vec4 color;

	UI_Class *uiClass;

	UI_Context *context;

	void AddToContext();

	void RemoveFromContext();

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
