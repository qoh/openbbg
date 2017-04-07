#ifndef _OPENBBG__UI__UI_CONTEXT_H_
#define _OPENBBG__UI__UI_CONTEXT_H_

namespace openbbg {

typedef struct UI_Context UI_Context;
typedef struct UI_Control UI_Control;

struct UI_Context
{
	/**
	 * Extent normalized by DPI scale
	 */
	glm::vec2 extentNormalized;

	/**
	 * Width and Height
	 */
	glm::vec2 extent;
	
	
	/**
	 * Root control
	 */

	UI_Control *root;


	UI_Context();

	~UI_Context();


	//--------------------------------------

	void SetRoot(UI_Control *ctrl);

	static deque<UI_Context *> s_contextList;

	static void CleanupAll();
};

}
#endif

// Definitions
#include <OpenBBG/UI/UI_Context_def.h>
