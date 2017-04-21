#include "stdafx.h"

// OpenBBG
#include <OpenBBG/UI/UI_Class_Control.h>
#include <OpenBBG/UI/UI_Control.h>

namespace openbbg {

IMPLEMENT_SINGLETON(UI_Class_Control);

UI_Class_Control::UI_Class_Control()
	: UI_Class()
{
}

UI_Class_Control::~UI_Class_Control()
{
}

UI_Control *
UI_Class_Control::Construct()
{
	auto ctrl = new UI_Control();
	ctrl->uiClass = this;
	return ctrl;
}

}
