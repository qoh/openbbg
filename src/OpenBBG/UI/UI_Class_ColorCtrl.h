#ifndef _OPENBBG__UI__UI_CLASS_COLORCTRL_H_
#define _OPENBBG__UI__UI_CLASS_COLORCTRL_H_

// OpenBBG
#include <OpenBBG/UI/UI_Class.h>

namespace openbbg {

typedef struct UI_Class_ColorCtrl UI_Class_ColorCtrl;

struct UI_Class_ColorCtrl
	: UI_Class
	, Singleton<UI_Class_ColorCtrl>
{
	UI_Class_ColorCtrl();

	~UI_Class_ColorCtrl();

	virtual UI_Control *Construct();
};

}
#endif
