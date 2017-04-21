#include "stdafx.h"

// OpenBBG
#include <OpenBBG/UI/UI_Class_ColorCtrl.h>
#include <OpenBBG/UI/UI_Control.h>
#include <OpenBBG/UI/UI_Component_ColorQuad.h>

namespace openbbg {

IMPLEMENT_SINGLETON(UI_Class_ColorCtrl);

UI_Class_ColorCtrl::UI_Class_ColorCtrl()
	: UI_Class()
{
}

UI_Class_ColorCtrl::~UI_Class_ColorCtrl()
{
}

UI_Control *
UI_Class_ColorCtrl::Construct()
{
	auto ctrl = new UI_Control();
	ctrl->uiClass = this;

	{
		auto compInst = UI_Component_ColorQuad::Get()->Construct();
		compInst->relativePosition = { 0.f, 0.f };
		compInst->extent = { 16.f, 16.f };
		compInst->zOffset = 0.f;
		compInst->zActual = -0.5f;
		ctrl->componentInstances.push_back(compInst);
	}

	return ctrl;
}

}
