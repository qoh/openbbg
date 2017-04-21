#ifndef _OPENBBG__UI__UI_CLASS_CONTROL_H_
#define _OPENBBG__UI__UI_CLASS_CONTROL_H_

// OpenBBG
#include <OpenBBG/UI/UI_Class.h>

namespace openbbg {

typedef struct UI_Class_Control UI_Class_Control;

struct UI_Class_Control
	: UI_Class
	, Singleton<UI_Class_Control>
{
	UI_Class_Control();

	~UI_Class_Control();

	virtual UI_Control *Construct();
};

}
#endif
