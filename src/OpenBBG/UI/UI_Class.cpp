#include "stdafx.h"

// OpenBBG
#include <OpenBBG/UI/UI_Class.h>

namespace openbbg {

vector<UI_Class *> UI_Class::s_classes;

UI_Class::UI_Class()
{
	s_classes.push_back(this);
}

UI_Class::~UI_Class()
{
	s_classes.erase(find(s_classes.begin(), s_classes.end(), this));
}

}
